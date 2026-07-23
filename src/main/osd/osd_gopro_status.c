/*
 * This file is part of Betaflight.
 *
 * Betaflight is free software. You can redistribute this software
 * and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * Betaflight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software.
 *
 * If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "platform.h"

#if defined(USE_OSD)

#include "drivers/serial.h"
#include "io/serial.h"
#include "common/gopro_json.h"
#include "common/printf.h"
#include "common/time.h"
#include "rx/rx.h"
#include "osd/osd.h"

#define GOPRO_INPUT_BUFFER_SIZE 2048
#define GOPRO_DISPLAY_BUFFER_SIZE 2048
#define GOPRO_BATTERY_VALUE_SIZE 8
#define GOPRO_RECORDING_VALUE_SIZE 16

static serialPort_t *goproSerialPort = NULL;
static char goproInputBuffer[GOPRO_INPUT_BUFFER_SIZE];
static uint16_t goproInputPos = 0;
static char goproDisplayBuffer[GOPRO_DISPLAY_BUFFER_SIZE];
static char goproBatteryValue[GOPRO_BATTERY_VALUE_SIZE];
static char goproRecordingValue[GOPRO_RECORDING_VALUE_SIZE];

/* Extract cached status fields from the latest GoPro status JSON line. */
static void osdGoproStatusUpdateCaches(void)
{
    const char *statusStart;
    const char *statusEnd;

    goproBatteryValue[0] = '\0';
    goproRecordingValue[0] = '\0';

    if (!goproJsonExtractObjectRange(goproDisplayBuffer, "status", &statusStart, &statusEnd)) {
        return;
    }

    goproJsonExtractValue(statusStart, statusEnd, "70", goproBatteryValue, sizeof(goproBatteryValue));
    goproJsonExtractValue(statusStart, statusEnd, "8", goproRecordingValue, sizeof(goproRecordingValue));
}

/* Send a JSON command line to the GoPro status port. */
bool osdGoproStatusSendCommand(uint16_t optionId,  uint16_t settingId)
{
    if (!goproSerialPort) {
        return false;
    }

    char command[192];
    tfp_sprintf(command, "{\"option\":%u,\"setting\":%u,\"ssid\":\"%s\",\"pass\":\"%s\"}", optionId, settingId, osdConfig()->gopro_ssid, osdConfig()->gopro_pass);

    serialWriteBuf(goproSerialPort, (const uint8_t *)command, strlen(command));
    serialWrite(goproSerialPort, '\n');
    return true;
}

/* Open the configured serial port and reset all GoPro status caches. */
bool osdGoproStatusInit(void)
{
    const serialPortConfig_t *portConfig = findSerialPortConfig(FUNCTION_OSD_GOPRO_STATUS);
    if (!portConfig) {
        return false;
    }

    uint8_t baudrateIndex = portConfig->gps_baudrateIndex;
    if (baudrateIndex >= BAUD_COUNT) {
        baudrateIndex = BAUD_57600;
    }
    const uint32_t baudrate = baudRates[baudrateIndex];

    goproSerialPort = openSerialPort(
        portConfig->identifier,
        FUNCTION_OSD_GOPRO_STATUS,
        NULL,
        NULL,
        baudrate,
            MODE_RXTX,
        SERIAL_NOT_INVERTED
    );

    if (!goproSerialPort) {
        return false;
    }

    memset(goproInputBuffer, 0, sizeof(goproInputBuffer));
    memset(goproDisplayBuffer, 0, sizeof(goproDisplayBuffer));
    memset(goproBatteryValue, 0, sizeof(goproBatteryValue));
    memset(goproRecordingValue, 0, sizeof(goproRecordingValue));
    goproInputPos = 0;

    return true;
}

/* Consume incoming GoPro status lines and update cached fields. */
void osdGoproStatusUpdate(timeUs_t currentTimeUs)
{
    UNUSED(currentTimeUs);

    if (!goproSerialPort) {
        return;
    }

    while (serialRxBytesWaiting(goproSerialPort)) {
        const uint8_t c = serialRead(goproSerialPort);

        if (c == '\n') {
            if (goproInputPos > 0) {
                const uint16_t copyLen = (goproInputPos < (GOPRO_DISPLAY_BUFFER_SIZE - 1)) ? goproInputPos : (GOPRO_DISPLAY_BUFFER_SIZE - 1);
                memcpy(goproDisplayBuffer, goproInputBuffer, copyLen);
                goproDisplayBuffer[copyLen] = '\0';
            } else {
                goproDisplayBuffer[0] = '\0';
            }

            osdGoproStatusUpdateCaches();
            goproInputPos = 0;
        } else if (c != '\r' && c != 0x00 && goproInputPos < (GOPRO_INPUT_BUFFER_SIZE - 1)) {
            goproInputBuffer[goproInputPos++] = c;
        }
    }

    /* Map the configured AUX channel to GoPro record start/stop commands. */
    {
        /* The configured AUX channel is 1-based, so convert it to the rcData index. */
        const uint8_t auxCfg = osdConfig()->gopro_aux_channel; // 1..N
        if (auxCfg > 0) {
            const int auxIndex = auxCfg + NON_AUX_CHANNEL_COUNT - 1;
            static bool prevAuxHigh = false;
            const float mid = rxConfig()->midrc;
            const bool curHigh = rcData[auxIndex] > mid;

            if (curHigh && !prevAuxHigh) {
                osdGoproStatusSendCommand(1, 8);
            } else if (!curHigh && prevAuxHigh) {
                osdGoproStatusSendCommand(0,8);
            } else {
                // no change
               // osdGoproStatusSendCommand(0,8);
            }
            prevAuxHigh = curHigh;
        }
    }
}

/* Return the latest raw GoPro status JSON payload. */
const char *osdGoproStatusGet(void)
{
    return goproDisplayBuffer;
}

/* Return the cached battery value parsed from the latest status line. */
const char *osdGoproStatusGetBattery(void)
{
    return goproBatteryValue;
}

/* Return the cached recording state parsed from the latest status line. */
const char *osdGoproStatusGetRecording(void)
{
    return goproRecordingValue;
}

#endif
