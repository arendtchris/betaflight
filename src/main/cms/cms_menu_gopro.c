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

#ifdef USE_CMS

#include <stdbool.h>
#include <stdint.h>

#include "common/time.h"
#include "cms/cms.h"
#include "cms/cms_types.h"
#include "drivers/time.h"
#include "msp/msp_gopro.h"

#define GOPRO_SETTING_RESOLUTION 2
#define GOPRO_SETTING_FPS 3
#define GOPRO_SETTING_VIDEO_LENS 121
#define GOPRO_SETTING_HYPERSMOOTH 135
#define GOPRO_MENU_SETTING_COUNT 4
#define GOPRO_SETTING_RETRY_INTERVAL_MS 500
#define GOPRO_SETTING_MAX_RETRIES 4
#define GOPRO_MENU_OPTION_COUNT GOPRO_MSP_MAX_SETTING_CAPABILITIES

typedef struct {
    uint8_t value;
    const char *label;
} goproMenuOptionLabel_t;

static const goproMenuOptionLabel_t resolutionOptionLabels[] = {
    { 1, "4K" }, { 4, "2.7K" }, { 6, "2.7K 4:3" }, { 7, "1440" },
    { 9, "1080" }, { 12, "720" }, { 18, "4K 4:3" }, { 21, "5.6K" },
    { 24, "5K" }, { 25, "5K 4:3" }, { 26, "5.3K 8:7" }, { 27, "5.3K 4:3" },
    { 28, "4K 8:7" }, { 31, "8K" }, { 35, "5.3K 21:9" }, { 36, "4K 21:9" },
    { 37, "4K 1:1" }, { 38, "900" }, { 39, "4K SPH" }, { 40, "8K 4:3" },
    { 44, "1440 4:3" }, { 100, "5.3K" }, { 107, "5.3K 8:7" }, { 108, "4K 8:7" },
    { 109, "4K 9:16" }, { 110, "1080 9:16" }, { 111, "2.7K 4:3" },
    { 112, "4K 4:3" }, { 113, "5.3K 4:3" },
};

static const goproMenuOptionLabel_t fpsOptionLabels[] = {
    { 0, "240" }, { 1, "120" }, { 2, "100" }, { 3, "90" },
    { 5, "60" }, { 6, "50" }, { 8, "30" }, { 9, "25" }, { 10, "24" },
    { 13, "200" }, { 15, "400" }, { 16, "360" }, { 17, "300" },
    { 18, "480" }, { 19, "960" }, { 20, "800" },
};

static const goproMenuOptionLabel_t videoLensOptionLabels[] = {
    { 0, "WIDE" }, { 2, "NARROW" }, { 3, "SUPERVIEW" }, { 4, "LINEAR" },
    { 7, "MAX SUPERVIEW" }, { 8, "HORIZON LEVEL" }, { 9, "HYPERVIEW" },
    { 10, "HORIZON LOCK" }, { 11, "MAX HYPERVIEW" }, { 12, "ULTRA SUPERVIEW" },
    { 13, "ULTRA WIDE" }, { 14, "ULTRA LINEAR" }, { 104, "ULTRA HYPERVIEW" },
};

static const goproMenuOptionLabel_t hypersmoothOptionLabels[] = {
    { 0, "OFF" }, { 1, "LOW" }, { 2, "HIGH" }, { 3, "BOOST" },
    { 4, "AUTO BOOST" }, { 100, "STANDARD" },
};

static const uint8_t menuSettingIds[GOPRO_MENU_SETTING_COUNT] = {
    GOPRO_SETTING_RESOLUTION, GOPRO_SETTING_FPS, GOPRO_SETTING_VIDEO_LENS, GOPRO_SETTING_HYPERSMOOTH
};
static uint8_t settingIndices[GOPRO_MENU_SETTING_COUNT];
static uint8_t settingObservedValues[GOPRO_MENU_SETTING_COUNT];
static bool settingObservedValuesValid[GOPRO_MENU_SETTING_COUNT];
static uint8_t settingValues[GOPRO_MENU_SETTING_COUNT][GOPRO_MENU_OPTION_COUNT];
static uint8_t settingOptionCounts[GOPRO_MENU_SETTING_COUNT];
static const char *settingLabels[GOPRO_MENU_SETTING_COUNT][GOPRO_MENU_OPTION_COUNT] = {
    { "WAIT" }, { "WAIT" }, { "WAIT" }, { "WAIT" }
};
static char settingFallbackLabels[GOPRO_MENU_SETTING_COUNT][GOPRO_MENU_OPTION_COUNT][6];
static uint32_t settingRequestMs;
static uint8_t settingRequestRetries;
static bool settingRequestActive[GOPRO_MENU_SETTING_COUNT];
static bool capabilitiesRequestActive[GOPRO_MENU_SETTING_COUNT];

static OSD_TAB_t settingTabs[GOPRO_MENU_SETTING_COUNT] = {
    { &settingIndices[0], 0, settingLabels[0] },
    { &settingIndices[1], 0, settingLabels[1] },
    { &settingIndices[2], 0, settingLabels[2] },
    { &settingIndices[3], 0, settingLabels[3] },
};

static int8_t cmsx_menuGoproFindSettingIndex(uint8_t settingId)
{
    for (uint8_t index = 0; index < GOPRO_MENU_SETTING_COUNT; index++) {
        if (menuSettingIds[index] == settingId) {
            return index;
        }
    }
    return -1;
}

static void cmsx_menuGoproSelectCurrentValue(uint8_t settingId, const uint8_t *values, uint8_t count, uint8_t *selectedIndex)
{
    uint8_t currentValue;
    if (!goproMspGetSetting(settingId, &currentValue)) {
        return;
    }

    for (uint8_t index = 0; index < count; index++) {
        if (values[index] == currentValue) {
            *selectedIndex = index;
            return;
        }
    }
}

static const char *cmsx_menuGoproFindOptionLabel(uint8_t settingId, uint8_t value, uint8_t index)
{
    const goproMenuOptionLabel_t *options = NULL;
    uint8_t optionCount = 0;
    const int8_t settingIndex = cmsx_menuGoproFindSettingIndex(settingId);
    if (settingIndex < 0) {
        return "?";
    }

    if (settingId == GOPRO_SETTING_RESOLUTION) {
        options = resolutionOptionLabels;
        optionCount = ARRAYLEN(resolutionOptionLabels);
    } else if (settingId == GOPRO_SETTING_FPS) {
        options = fpsOptionLabels;
        optionCount = ARRAYLEN(fpsOptionLabels);
    } else if (settingId == GOPRO_SETTING_VIDEO_LENS) {
        options = videoLensOptionLabels;
        optionCount = ARRAYLEN(videoLensOptionLabels);
    } else if (settingId == GOPRO_SETTING_HYPERSMOOTH) {
        options = hypersmoothOptionLabels;
        optionCount = ARRAYLEN(hypersmoothOptionLabels);
    }

    for (uint8_t option = 0; option < optionCount; option++) {
        if (options[option].value == value) {
            return options[option].label;
        }
    }

    char *fallback = settingFallbackLabels[(uint8_t)settingIndex][index];
    fallback[0] = 'I';
    fallback[1] = 'D';
    if (value >= 100) {
        fallback[2] = '0' + value / 100;
        fallback[3] = '0' + (value / 10) % 10;
        fallback[4] = '0' + value % 10;
        fallback[5] = '\0';
    } else if (value >= 10) {
        fallback[2] = '0' + value / 10;
        fallback[3] = '0' + value % 10;
        fallback[4] = '\0';
    } else {
        fallback[2] = '0' + value;
        fallback[3] = '\0';
    }
    return settingFallbackLabels[(uint8_t)settingIndex][index];
}

static bool cmsx_menuGoproBuildCapabilities(uint8_t settingId)
{
    const int8_t settingIndex = cmsx_menuGoproFindSettingIndex(settingId);
    if (settingIndex < 0) {
        return false;
    }

    uint8_t count = 0;
    const uint8_t *values = goproMspGetSettingCapabilities(settingId, &count);
    if (!values) {
        return false;
    }

    const uint8_t settingSlot = (uint8_t)settingIndex;
    settingOptionCounts[settingSlot] = count;
    for (uint8_t index = 0; index < count; index++) {
        settingValues[settingSlot][index] = values[index];
        settingLabels[settingSlot][index] = cmsx_menuGoproFindOptionLabel(settingId, values[index], index);
    }
    if (count == 0) {
        settingLabels[settingSlot][0] = "NONE";
    }
    if (settingIndices[settingSlot] >= count) {
        settingIndices[settingSlot] = 0;
    }
    settingTabs[settingSlot].max = count > 0 ? count - 1 : 0;
    cmsx_menuGoproSelectCurrentValue(settingId, settingValues[settingSlot], count, &settingIndices[settingSlot]);
    return true;
}

static bool cmsx_menuGoproSyncSetting(uint8_t settingId)
{
    const int8_t settingIndex = cmsx_menuGoproFindSettingIndex(settingId);
    if (settingIndex < 0) {
        return false;
    }
    const uint8_t settingSlot = (uint8_t)settingIndex;

    uint8_t currentValue;
    if (!goproMspGetSetting(settingId, &currentValue)
        || (settingObservedValuesValid[settingSlot] && currentValue == settingObservedValues[settingSlot])) {
        return false;
    }

    settingObservedValues[settingSlot] = currentValue;
    settingObservedValuesValid[settingSlot] = true;
    cmsx_menuGoproSelectCurrentValue(settingId, settingValues[settingSlot], settingOptionCounts[settingSlot], &settingIndices[settingSlot]);
    return true;
}

static void cmsx_menuGoproRequestCapabilities(uint8_t settingSlot)
{
    if (settingSlot >= GOPRO_MENU_SETTING_COUNT) {
        return;
    }
    goproMspTakeSettingCapabilitiesUpdate(menuSettingIds[settingSlot]);
    capabilitiesRequestActive[settingSlot] = goproMspSendGetSettingCapabilities(menuSettingIds[settingSlot]);
    settingRequestMs = millis();
    settingRequestRetries = 0;
}

static const void *cmsx_menuGoproSetSetting(displayPort_t *display, const void *self, uint8_t settingSlot)
{
    UNUSED(display);
    UNUSED(self);
    if (settingSlot < GOPRO_MENU_SETTING_COUNT && settingIndices[settingSlot] < settingOptionCounts[settingSlot]) {
        const uint8_t settingId = menuSettingIds[settingSlot];
        const uint8_t value = settingValues[settingSlot][settingIndices[settingSlot]];
        goproMspSendSetSetting(settingId, value);
        settingRequestActive[settingSlot] = goproMspSendGetSetting(settingId);
        if (settingSlot == 0) {
            settingRequestActive[1] = goproMspSendGetSetting(menuSettingIds[1]);
        }
        if (settingSlot + 1 < GOPRO_MENU_SETTING_COUNT) {
            cmsx_menuGoproRequestCapabilities(settingSlot + 1);
        }
        settingRequestMs = millis();
        settingRequestRetries = 0;
    }
    return NULL;
}

static const void *cmsx_menuGoproSetResolution(displayPort_t *display, const void *self)
{
    return cmsx_menuGoproSetSetting(display, self, 0);
}

static const void *cmsx_menuGoproSetFps(displayPort_t *display, const void *self)
{
    return cmsx_menuGoproSetSetting(display, self, 1);
}

static const void *cmsx_menuGoproSetVideoLens(displayPort_t *display, const void *self)
{
    return cmsx_menuGoproSetSetting(display, self, 2);
}

static const void *cmsx_menuGoproSetHypersmooth(displayPort_t *display, const void *self)
{
    return cmsx_menuGoproSetSetting(display, self, 3);
}

static const void *cmsx_menuGoproOnEnter(displayPort_t *display)
{
    UNUSED(display);

    for (uint8_t settingSlot = 0; settingSlot < GOPRO_MENU_SETTING_COUNT; settingSlot++) {
        goproMspTakeSettingCapabilitiesUpdate(menuSettingIds[settingSlot]);
        cmsx_menuGoproBuildCapabilities(menuSettingIds[settingSlot]);
        settingRequestActive[settingSlot] = goproMspSendGetSetting(menuSettingIds[settingSlot]);
        capabilitiesRequestActive[settingSlot] = false;
    }
    settingRequestMs = millis();
    settingRequestRetries = 0;
    cmsx_menuGoproRequestCapabilities(0);
    return NULL;
}

static const void *cmsx_menuGoproOnDisplayUpdate(displayPort_t *display, const OSD_Entry *selected)
{
    UNUSED(display);
    UNUSED(selected);

    for (uint8_t settingSlot = 0; settingSlot < GOPRO_MENU_SETTING_COUNT; settingSlot++) {
        if (cmsx_menuGoproSyncSetting(menuSettingIds[settingSlot])) {
            settingRequestActive[settingSlot] = false;
        }
        if (goproMspTakeSettingCapabilitiesUpdate(menuSettingIds[settingSlot])) {
            cmsx_menuGoproBuildCapabilities(menuSettingIds[settingSlot]);
            capabilitiesRequestActive[settingSlot] = false;
            cmsx_menuGoproRequestCapabilities(settingSlot + 1);
        }
    }

    bool requestActive = false;
    for (uint8_t settingSlot = 0; settingSlot < GOPRO_MENU_SETTING_COUNT; settingSlot++) {
        requestActive |= settingRequestActive[settingSlot] || capabilitiesRequestActive[settingSlot];
    }
    if (requestActive
        && settingRequestRetries < GOPRO_SETTING_MAX_RETRIES
        && cmp32(millis(), settingRequestMs) >= GOPRO_SETTING_RETRY_INTERVAL_MS) {
        settingRequestMs = millis();
        settingRequestRetries++;
        for (uint8_t settingSlot = 0; settingSlot < GOPRO_MENU_SETTING_COUNT; settingSlot++) {
            if (settingRequestActive[settingSlot]) {
                settingRequestActive[settingSlot] = goproMspSendGetSetting(menuSettingIds[settingSlot]);
            }
            if (capabilitiesRequestActive[settingSlot]) {
                capabilitiesRequestActive[settingSlot] = goproMspSendGetSettingCapabilities(menuSettingIds[settingSlot]);
            }
        }
    }

    return NULL;
}

static const OSD_Entry menuGoproEntries[] = {
    { "RESOLUTION", OME_TAB, cmsx_menuGoproSetResolution, &settingTabs[0] },
    { "FPS", OME_TAB, cmsx_menuGoproSetFps, &settingTabs[1] },
    { "VIDEO LENS", OME_TAB, cmsx_menuGoproSetVideoLens, &settingTabs[2] },
    { "HYPERSMOOTH", OME_TAB, cmsx_menuGoproSetHypersmooth, &settingTabs[3] },
    { "BACK", OME_Back, NULL, NULL },
    { NULL, OME_END, NULL, NULL }
};

CMS_Menu cmsx_menuGopro = {
#ifdef CMS_MENU_DEBUG
    .GUARD_text = "MENUGOPRO",
    .GUARD_type = OME_MENU,
#endif
    .onEnter = cmsx_menuGoproOnEnter,
    .onExit = NULL,
    .onDisplayUpdate = cmsx_menuGoproOnDisplayUpdate,
    .entries = menuGoproEntries,
};

#endif