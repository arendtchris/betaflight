/*
 * This file is part of Cleanflight and Betaflight.
 *
 * Cleanflight and Betaflight are free software. You can redistribute
 * this software and/or modify this software under the terms of the
 * GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software.
 *
 * If not, see <http://www.gnu.org/licenses/>.
 */

#include "platform.h"

#include <stdbool.h>
#include <stdint.h>

#include "common/streambuf.h"
#include "io/serial.h"
#include "msp/msp_gopro.h"
#include "msp/msp_protocol_v2_betaflight.h"
#include "msp/msp_serial.h"
#include "pg/msp.h"

#define GOPRO_SETTING_RESOLUTION 2
#define GOPRO_SETTING_FPS 3
#define GOPRO_SETTING_VIDEO_LENS 121
#define GOPRO_SETTING_HYPERSMOOTH 135

typedef struct {
    uint8_t value;
    uint8_t revision;
    bool valid;
} goproMspSetting_t;

typedef struct {
    uint8_t values[GOPRO_MSP_MAX_SETTING_CAPABILITIES];
    uint8_t count;
    volatile bool updatePending;
    bool valid;
} goproMspCapabilities_t;

static goproMspSetting_t resolutionSetting;
static goproMspSetting_t fpsSetting;
static goproMspSetting_t videoLensSetting;
static goproMspSetting_t hypersmoothSetting;
static goproMspCapabilities_t resolutionCapabilities;
static goproMspCapabilities_t fpsCapabilities;
static goproMspCapabilities_t videoLensCapabilities;
static goproMspCapabilities_t hypersmoothCapabilities;

static goproMspSetting_t *goproMspFindSetting(uint8_t settingId)
{
    switch (settingId) {
    case GOPRO_SETTING_RESOLUTION:
        return &resolutionSetting;
    case GOPRO_SETTING_FPS:
        return &fpsSetting;
    case GOPRO_SETTING_VIDEO_LENS:
        return &videoLensSetting;
    case GOPRO_SETTING_HYPERSMOOTH:
        return &hypersmoothSetting;
    default:
        return NULL;
    }
}

static goproMspCapabilities_t *goproMspFindCapabilities(uint8_t settingId)
{
    switch (settingId) {
    case GOPRO_SETTING_RESOLUTION:
        return &resolutionCapabilities;
    case GOPRO_SETTING_FPS:
        return &fpsCapabilities;
    case GOPRO_SETTING_VIDEO_LENS:
        return &videoLensCapabilities;
    case GOPRO_SETTING_HYPERSMOOTH:
        return &hypersmoothCapabilities;
    default:
        return NULL;
    }
}

static serialPortIdentifier_e goproMspGetPort(void)
{
    return (serialPortIdentifier_e)mspConfig()->gopro_msp_uart;
}

static bool goproMspSend(uint16_t command, uint8_t *payload, uint8_t payloadSize)
{
    const serialPortIdentifier_e port = goproMspGetPort();
    if (port == SERIAL_PORT_NONE) {
        return false;
    }

    return mspSerialPush(port, command, payload, payloadSize, MSP_DIRECTION_REQUEST, MSP_V2_NATIVE) > 0;
}

bool goproMspSendGetSetting(uint8_t settingId)
{
    return goproMspSend(MSP2_GP_GET_SETTINGS, &settingId, sizeof(settingId));
}

bool goproMspSendSetSetting(uint8_t settingId, uint8_t value)
{
    uint8_t payload[] = { settingId, value };
    return goproMspSend(MSP2_GP_SET_SETTING, payload, sizeof(payload));
}

bool goproMspSendGetSettingCapabilities(uint8_t settingId)
{
    return goproMspSend(MSP2_GP_GET_SETTING_CAPABILITIES, &settingId, sizeof(settingId));
}

bool goproMspGetSetting(uint8_t settingId, uint8_t *value)
{
    goproMspSetting_t *setting = goproMspFindSetting(settingId);
    if (!setting || !setting->valid || !value) {
        return false;
    }

    *value = setting->value;
    return true;
}

uint8_t goproMspGetSettingRevision(uint8_t settingId)
{
    goproMspSetting_t *setting = goproMspFindSetting(settingId);
    return setting ? setting->revision : 0;
}

const uint8_t *goproMspGetSettingCapabilities(uint8_t settingId, uint8_t *count)
{
    goproMspCapabilities_t *capabilities = goproMspFindCapabilities(settingId);
    if (!capabilities || !capabilities->valid || !count) {
        return NULL;
    }

    *count = capabilities->count;
    return capabilities->values;
}

bool goproMspTakeSettingCapabilitiesUpdate(uint8_t settingId)
{
    goproMspCapabilities_t *capabilities = goproMspFindCapabilities(settingId);
    if (!capabilities || !capabilities->updatePending) {
        return false;
    }

    capabilities->updatePending = false;
    return true;
}

void goproMspProcessReply(mspPacket_t *reply)
{
    if (!reply) {
        return;
    }

    if (reply->cmd == MSP2_GP_SETTINGS_REPORT && sbufBytesRemaining(&reply->buf) >= 2) {
        const uint8_t settingId = sbufReadU8(&reply->buf);
        const uint8_t value = sbufReadU8(&reply->buf);
        goproMspSetting_t *setting = goproMspFindSetting(settingId);
        if (!setting) {
            return;
        }

        setting->value = value;
        setting->valid = true;
        setting->revision++;
        return;
    }

    if (reply->cmd != MSP2_GP_SETTING_CAPABILITIES_REPORT || sbufBytesRemaining(&reply->buf) < 2) {
        return;
    }

    const uint8_t settingId = sbufReadU8(&reply->buf);
    const uint8_t count = sbufReadU8(&reply->buf);
    if (count > GOPRO_MSP_MAX_SETTING_CAPABILITIES || sbufBytesRemaining(&reply->buf) != count) {
        return;
    }

    goproMspCapabilities_t *capabilities = goproMspFindCapabilities(settingId);
    if (!capabilities) {
        return;
    }

    for (uint8_t index = 0; index < count; index++) {
        capabilities->values[index] = sbufReadU8(&reply->buf);
    }
    capabilities->count = count;
    capabilities->valid = true;
    capabilities->updatePending = true;
}