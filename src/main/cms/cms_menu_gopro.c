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

static uint8_t resolutionIndex;
static uint8_t fpsIndex;
static uint8_t resolutionObservedValue;
static uint8_t fpsObservedValue;
static bool resolutionObservedValueValid;
static bool fpsObservedValueValid;
static uint8_t resolutionCapabilitiesRevision;
static uint8_t fpsCapabilitiesRevision;
static uint8_t resolutionValues[GOPRO_MENU_OPTION_COUNT];
static uint8_t fpsValues[GOPRO_MENU_OPTION_COUNT];
static uint8_t resolutionOptionCount;
static uint8_t fpsOptionCount;
static const char *resolutionLabels[GOPRO_MENU_OPTION_COUNT] = { "WAIT" };
static const char *fpsLabels[GOPRO_MENU_OPTION_COUNT] = { "WAIT" };
static char resolutionFallbackLabels[GOPRO_MENU_OPTION_COUNT][6];
static char fpsFallbackLabels[GOPRO_MENU_OPTION_COUNT][6];
static uint32_t settingRequestMs;
static uint8_t settingRequestRetries;
static bool resolutionRequestActive;
static bool fpsRequestActive;
static bool resolutionCapabilitiesRequestActive;
static bool fpsCapabilitiesRequestActive;

static OSD_TAB_t resolutionTab = { &resolutionIndex, 0, resolutionLabels };
static OSD_TAB_t fpsTab = { &fpsIndex, 0, fpsLabels };

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
    const goproMenuOptionLabel_t *options;
    uint8_t optionCount;
    char (*fallbackLabels)[6];

    if (settingId == GOPRO_SETTING_RESOLUTION) {
        options = resolutionOptionLabels;
        optionCount = ARRAYLEN(resolutionOptionLabels);
        fallbackLabels = resolutionFallbackLabels;
    } else {
        options = fpsOptionLabels;
        optionCount = ARRAYLEN(fpsOptionLabels);
        fallbackLabels = fpsFallbackLabels;
    }

    for (uint8_t option = 0; option < optionCount; option++) {
        if (options[option].value == value) {
            return options[option].label;
        }
    }

    char *fallback = fallbackLabels[index];
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
    return fallbackLabels[index];
}

static bool cmsx_menuGoproBuildCapabilities(uint8_t settingId)
{
    uint8_t count = 0;
    const uint8_t *values = goproMspGetSettingCapabilities(settingId, &count);
    if (!values) {
        return false;
    }

    uint8_t *menuValues;
    const char **menuLabels;
    uint8_t *menuCount;
    OSD_TAB_t *menuTab;
    uint8_t *selectedIndex;

    if (settingId == GOPRO_SETTING_RESOLUTION) {
        menuValues = resolutionValues;
        menuLabels = resolutionLabels;
        menuCount = &resolutionOptionCount;
        menuTab = &resolutionTab;
        selectedIndex = &resolutionIndex;
    } else {
        menuValues = fpsValues;
        menuLabels = fpsLabels;
        menuCount = &fpsOptionCount;
        menuTab = &fpsTab;
        selectedIndex = &fpsIndex;
    }

    *menuCount = count;
    for (uint8_t index = 0; index < count; index++) {
        menuValues[index] = values[index];
        menuLabels[index] = cmsx_menuGoproFindOptionLabel(settingId, values[index], index);
    }
    if (count == 0) {
        menuLabels[0] = "NONE";
    }
    if (*selectedIndex >= count) {
        *selectedIndex = 0;
    }
    menuTab->max = count > 0 ? count - 1 : 0;
    cmsx_menuGoproSelectCurrentValue(settingId, menuValues, count, selectedIndex);
    return true;
}

static void cmsx_menuGoproSyncSetting(uint8_t settingId, const uint8_t *values, uint8_t valueCount, uint8_t *selectedIndex)
{
    uint8_t *observedValue;
    bool *observedValueValid;
    if (settingId == GOPRO_SETTING_RESOLUTION) {
        observedValue = &resolutionObservedValue;
        observedValueValid = &resolutionObservedValueValid;
    } else {
        observedValue = &fpsObservedValue;
        observedValueValid = &fpsObservedValueValid;
    }

    uint8_t currentValue;
    if (!goproMspGetSetting(settingId, &currentValue)
        || (*observedValueValid && currentValue == *observedValue)) {
        return;
    }

    *observedValue = currentValue;
    *observedValueValid = true;
    cmsx_menuGoproSelectCurrentValue(settingId, values, valueCount, selectedIndex);
}

static const void *cmsx_menuGoproSetResolution(displayPort_t *display, const void *self)
{
    UNUSED(display);
    UNUSED(self);
    resolutionRequestActive = false;
    resolutionCapabilitiesRequestActive = false;
    if (resolutionIndex < resolutionOptionCount) {
        goproMspSendSetSetting(GOPRO_SETTING_RESOLUTION, resolutionValues[resolutionIndex]);
        resolutionRequestActive = goproMspSendGetSetting(GOPRO_SETTING_RESOLUTION);
        fpsRequestActive = goproMspSendGetSetting(GOPRO_SETTING_FPS);
        fpsCapabilitiesRevision = goproMspGetSettingCapabilitiesRevision(GOPRO_SETTING_FPS);
        fpsCapabilitiesRequestActive = goproMspSendGetSettingCapabilities(GOPRO_SETTING_FPS);
        settingRequestMs = millis();
        settingRequestRetries = 0;
    }
    return NULL;
}

static const void *cmsx_menuGoproSetFps(displayPort_t *display, const void *self)
{
    UNUSED(display);
    UNUSED(self);
    fpsRequestActive = false;
    if (fpsIndex < fpsOptionCount) {
        goproMspSendSetSetting(GOPRO_SETTING_FPS, fpsValues[fpsIndex]);
        fpsRequestActive = true;
        settingRequestMs = millis();
        settingRequestRetries = 0;
    }
    return NULL;
}

static const void *cmsx_menuGoproOnEnter(displayPort_t *display)
{
    UNUSED(display);

    resolutionCapabilitiesRevision = goproMspGetSettingCapabilitiesRevision(GOPRO_SETTING_RESOLUTION);
    cmsx_menuGoproBuildCapabilities(GOPRO_SETTING_RESOLUTION);
    cmsx_menuGoproBuildCapabilities(GOPRO_SETTING_FPS);
    settingRequestMs = millis();
    settingRequestRetries = 0;
    resolutionRequestActive = goproMspSendGetSetting(GOPRO_SETTING_RESOLUTION);
    fpsRequestActive = goproMspSendGetSetting(GOPRO_SETTING_FPS);
    resolutionCapabilitiesRequestActive = goproMspSendGetSettingCapabilities(GOPRO_SETTING_RESOLUTION);
    fpsCapabilitiesRequestActive = false;
    return NULL;
}

static const void *cmsx_menuGoproOnDisplayUpdate(displayPort_t *display, const OSD_Entry *selected)
{
    UNUSED(display);
    UNUSED(selected);

    cmsx_menuGoproSyncSetting(GOPRO_SETTING_RESOLUTION, resolutionValues, resolutionOptionCount, &resolutionIndex);
    cmsx_menuGoproSyncSetting(GOPRO_SETTING_FPS, fpsValues, fpsOptionCount, &fpsIndex);

    const uint8_t newResolutionCapabilitiesRevision = goproMspGetSettingCapabilitiesRevision(GOPRO_SETTING_RESOLUTION);
    if (newResolutionCapabilitiesRevision != resolutionCapabilitiesRevision) {
        resolutionCapabilitiesRevision = newResolutionCapabilitiesRevision;
        cmsx_menuGoproBuildCapabilities(GOPRO_SETTING_RESOLUTION);
        resolutionCapabilitiesRequestActive = false;
        fpsCapabilitiesRevision = goproMspGetSettingCapabilitiesRevision(GOPRO_SETTING_FPS);
        fpsCapabilitiesRequestActive = goproMspSendGetSettingCapabilities(GOPRO_SETTING_FPS);
        settingRequestMs = millis();
        settingRequestRetries = 0;
    }
    const uint8_t newFpsCapabilitiesRevision = goproMspGetSettingCapabilitiesRevision(GOPRO_SETTING_FPS);
    if (newFpsCapabilitiesRevision != fpsCapabilitiesRevision) {
        fpsCapabilitiesRevision = newFpsCapabilitiesRevision;
        cmsx_menuGoproBuildCapabilities(GOPRO_SETTING_FPS);
        fpsCapabilitiesRequestActive = false;
    }

    if ((resolutionRequestActive || fpsRequestActive || resolutionCapabilitiesRequestActive || fpsCapabilitiesRequestActive)
        && settingRequestRetries < GOPRO_SETTING_MAX_RETRIES
        && cmp32(millis(), settingRequestMs) >= GOPRO_SETTING_RETRY_INTERVAL_MS) {
        settingRequestMs = millis();
        settingRequestRetries++;
        if (resolutionRequestActive) {
            resolutionRequestActive = goproMspSendGetSetting(GOPRO_SETTING_RESOLUTION);
        }
        if (fpsRequestActive) {
            fpsRequestActive = goproMspSendGetSetting(GOPRO_SETTING_FPS);
        }
        if (resolutionCapabilitiesRequestActive) {
            resolutionCapabilitiesRequestActive = goproMspSendGetSettingCapabilities(GOPRO_SETTING_RESOLUTION);
        }
        if (fpsCapabilitiesRequestActive) {
            fpsCapabilitiesRequestActive = goproMspSendGetSettingCapabilities(GOPRO_SETTING_FPS);
        }
    }

    return NULL;
}

static const OSD_Entry menuGoproEntries[] = {
    { "RESOLUTION", OME_TAB, cmsx_menuGoproSetResolution, &resolutionTab },
    { "FPS", OME_TAB, cmsx_menuGoproSetFps, &fpsTab },
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