/*
 * This file is part of Cleanflight and Betaflight.
 *
 * Cleanflight and Betaflight are free software. You can redistribute
 * this software and/or modify this software under the terms of the
 * GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * Cleanflight and Betaflight are distributed in the hope that they
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software.
 *
 * If not, see <http://www.gnu.org/licenses/>.
 */

#include "platform.h"

#if defined(USE_OSD) && defined(USE_CMS)

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "common/printf.h"
#include "common/gopro_json.h"
#include "cms/cms.h"
#include "cms/cms_types.h"
#include "cms/cms_menu_gopro.h"
#include "osd/osd_gopro_status.h"

#define GOPRO_STATUS_BATTERY_TEXT_SIZE 8
#define GOPRO_STATUS_RECORDING_TEXT_SIZE 16
#define GOPRO_STATUS_LINK_TEXT_SIZE 16
#define GOPRO_SETTING_RECORD 8
#define GOPRO_SETTING_RESOLUTION 2
#define GOPRO_SETTING_FPS 3
#define GOPRO_SETTING_LENS 121
#define GOPRO_SETTING_HYPERSMOOTH 135

static const char * const goproRecordNames[] = {
    "STOP", "START"
};

static const uint16_t goproRecordOptions[] = {
    0, 1
};

static const char * const goproResolutionNames[] = {
    "5.3K", "5.3K 8:7", "5.3K 4:3", "4K", "4K 8:7", "4K 4:3", "2.7K", "2.7K 4:3", "1080"
};

static const uint16_t goproResolutionOptions[] = {
    100, 26, 27, 1, 28, 18, 4, 6, 9
};

static const char * const goproFpsNames[] = {
    "240", "200", "120", "100", "60 ", "50 ", "30 ", "25 ", "24 "
};

static const uint16_t goproFpsOptions[] = {
    0, 13, 1, 2, 5, 6, 8, 9, 10
};

static const char * const goproLensNames[] = {
    "WIDE", "SUPERVIEW", "LINEAR", "MAX SV", "LIN HL", "HYPERVIEW", "LIN LOCK"
};

static const uint16_t goproLensOptions[] = {
    0, 3, 4, 7, 8, 9, 10
};

static const char * const goproHypersmoothNames[] = {
    "OFF", "LOW", "BOOST", "AUTO"
};

static const uint16_t goproHypersmoothOptions[] = {
    0, 1, 3, 4
};

static char goproStatusBatteryText[GOPRO_STATUS_BATTERY_TEXT_SIZE];
static char goproStatusRecordingText[GOPRO_STATUS_RECORDING_TEXT_SIZE];
static char goproStatusLinkText[GOPRO_STATUS_LINK_TEXT_SIZE];
static uint8_t goproRecordIndex = 0;
static uint8_t goproResolutionIndex = 2;
static uint8_t goproFpsIndex = 4;
static uint8_t goproLensIndex = 1;
static uint8_t goproHypersmoothIndex = 0;
static OSD_TAB_t goproCmsEntRecord = { &goproRecordIndex, ARRAYLEN(goproRecordNames) - 1, goproRecordNames };
static OSD_TAB_t goproCmsEntResolution = { &goproResolutionIndex, ARRAYLEN(goproResolutionNames) - 1, goproResolutionNames };
static OSD_TAB_t goproCmsEntFps = { &goproFpsIndex, ARRAYLEN(goproFpsNames) - 1, goproFpsNames };
static OSD_TAB_t goproCmsEntLens = { &goproLensIndex, ARRAYLEN(goproLensNames) - 1, goproLensNames };
static OSD_TAB_t goproCmsEntHypersmooth = { &goproHypersmoothIndex, ARRAYLEN(goproHypersmoothNames) - 1, goproHypersmoothNames };

// Sends a raw GoPro command string over the existing status/control UART link.
static const void *cmsx_menuGoproSendCommand(displayPort_t *pDisp, const void *ptr)
{
    UNUSED(pDisp);

    const char *commandParam = (const char *)ptr;
    if (commandParam && commandParam[0]) {
        osdGoproStatusSendCommand(commandParam);
    }

    return NULL;
}

// Finds the index of a specific option value inside an option table.
static int8_t cmsx_menuGoproFindOptionIndex(const uint16_t *options, uint8_t optionCount, uint16_t optionValue)
{
    for (uint8_t index = 0; index < optionCount; index++) {
        if (options[index] == optionValue) {
            return index;
        }
    }

    return -1;
}

// Syncs a CMS tab index with the current value of a GoPro setting from state JSON.
static void cmsx_menuGoproSyncTabFromStatus(uint16_t settingId, const uint16_t *options, uint8_t optionCount, uint8_t *selectedIndex)
{
    uint16_t optionValue;
    const char *statusText = osdGoproStatusGet();

    if (!selectedIndex || !statusText || !statusText[0]) {
        return;
    }

    if (!goproJsonGetSettingOption(statusText, settingId, &optionValue)) {
        return;
    }

    const int8_t matchedIndex = cmsx_menuGoproFindOptionIndex(options, optionCount, optionValue);
    if (matchedIndex >= 0) {
        *selectedIndex = (uint8_t)matchedIndex;
    }
}

// Syncs the record tab from status key 8, which reflects current recording state.
static void cmsx_menuGoproSyncRecordTabFromStatus(void)
{
    uint16_t recordingValue;
    const char *statusText = osdGoproStatusGet();
    const char *statusStart;
    const char *statusEnd;

    if (!statusText || !statusText[0]) {
        return;
    }

    if (!goproJsonExtractObjectRange(statusText, "status", &statusStart, &statusEnd)) {
        return;
    }

    if (!goproJsonExtractUint16(statusStart, statusEnd, "8", &recordingValue)) {
        return;
    }

    const int8_t matchedIndex = cmsx_menuGoproFindOptionIndex(goproRecordOptions, ARRAYLEN(goproRecordOptions), recordingValue);
    if (matchedIndex >= 0) {
        goproRecordIndex = (uint8_t)matchedIndex;
    }
}

// Builds and sends a setting command using the currently selected tab option.
static const void *cmsx_menuGoproSendIndexedSetting(displayPort_t *pDisp, uint8_t *selectedIndex, const uint16_t *options, uint8_t optionCount, uint16_t settingId)
{
    if (!selectedIndex || !options || !optionCount) {
        return NULL;
    }

    if (*selectedIndex >= optionCount) {
        *selectedIndex = 0;
    }

    char command[32];
    tfp_sprintf(command, "option=%u&setting=%u", options[*selectedIndex], settingId);
    return cmsx_menuGoproSendCommand(pDisp, command);
}

// Applies the selected record state (start/stop).
static const void *cmsx_menuGoproSetRecord(displayPort_t *pDisp, const void *self)
{
    UNUSED(self);

    return cmsx_menuGoproSendIndexedSetting(pDisp, &goproRecordIndex, goproRecordOptions, ARRAYLEN(goproRecordOptions), GOPRO_SETTING_RECORD);
}

// Applies the selected video resolution.
static const void *cmsx_menuGoproSetResolution(displayPort_t *pDisp, const void *self)
{
    UNUSED(self);

    return cmsx_menuGoproSendIndexedSetting(pDisp, &goproResolutionIndex, goproResolutionOptions, ARRAYLEN(goproResolutionOptions), GOPRO_SETTING_RESOLUTION);
}

// Applies the selected frame-rate option.
static const void *cmsx_menuGoproSetFps(displayPort_t *pDisp, const void *self)
{
    UNUSED(self);

    return cmsx_menuGoproSendIndexedSetting(pDisp, &goproFpsIndex, goproFpsOptions, ARRAYLEN(goproFpsOptions), GOPRO_SETTING_FPS);
}

// Applies the selected lens mode.
static const void *cmsx_menuGoproSetLens(displayPort_t *pDisp, const void *self)
{
    UNUSED(self);

    return cmsx_menuGoproSendIndexedSetting(pDisp, &goproLensIndex, goproLensOptions, ARRAYLEN(goproLensOptions), GOPRO_SETTING_LENS);
}

// Applies the selected HyperSmooth mode.
static const void *cmsx_menuGoproSetHypersmooth(displayPort_t *pDisp, const void *self)
{
    UNUSED(self);

    return cmsx_menuGoproSendIndexedSetting(pDisp, &goproHypersmoothIndex, goproHypersmoothOptions, ARRAYLEN(goproHypersmoothOptions), GOPRO_SETTING_HYPERSMOOTH);
}

// Initializes menu tab values from the latest GoPro state when entering the menu.
static const void *cmsx_menuGoproOnEnter(displayPort_t *pDisp)
{
    UNUSED(pDisp);

    cmsx_menuGoproSyncRecordTabFromStatus();
    cmsx_menuGoproSyncTabFromStatus(GOPRO_SETTING_RESOLUTION, goproResolutionOptions, ARRAYLEN(goproResolutionOptions), &goproResolutionIndex);
    cmsx_menuGoproSyncTabFromStatus(GOPRO_SETTING_FPS, goproFpsOptions, ARRAYLEN(goproFpsOptions), &goproFpsIndex);
    cmsx_menuGoproSyncTabFromStatus(GOPRO_SETTING_LENS, goproLensOptions, ARRAYLEN(goproLensOptions), &goproLensIndex);
    cmsx_menuGoproSyncTabFromStatus(GOPRO_SETTING_HYPERSMOOTH, goproHypersmoothOptions, ARRAYLEN(goproHypersmoothOptions), &goproHypersmoothIndex);

    return NULL;
}

// Safely copies status text into a fixed-size menu buffer with fallback handling.
static void cmsx_menuGoproCopyStatusText(char *dst, size_t dstSize, const char *src, const char *fallback)
{
    if (!dst || !dstSize) {
        return;
    }

    const char *value = (src && src[0]) ? src : fallback;
    if (!value) {
        value = "";
    }

    strncpy(dst, value, dstSize - 1);
    dst[dstSize - 1] = '\0';
}

// Refreshes dynamic status strings shown in the status submenu.
static void cmsx_menuGoproRefreshStatus(void)
{
    cmsx_menuGoproCopyStatusText(goproStatusBatteryText, sizeof(goproStatusBatteryText), osdGoproStatusGetBattery(), "--");
    cmsx_menuGoproCopyStatusText(goproStatusRecordingText, sizeof(goproStatusRecordingText), osdGoproStatusGetRecording(), "--");
    cmsx_menuGoproCopyStatusText(goproStatusLinkText, sizeof(goproStatusLinkText), osdGoproStatusGet()[0] ? "ONLINE" : NULL, "WAITING");
}

// Updates status strings once when entering the status submenu.
static const void *cmsx_menuGoproStatusOnEnter(displayPort_t *pDisp)
{
    UNUSED(pDisp);

    cmsx_menuGoproRefreshStatus();

    return NULL;
}

// Keeps status strings updated while the status submenu is displayed.
static const void *cmsx_menuGoproStatusOnDisplayUpdate(displayPort_t *pDisp, const OSD_Entry *selected)
{
    UNUSED(pDisp);
    UNUSED(selected);

    cmsx_menuGoproRefreshStatus();

    return NULL;
}

static const OSD_Entry cmsx_menuGoproStatusEntries[] =
{
    {"- STATUS -", OME_Label, NULL, NULL},
    {"BATT", OME_Label | DYNAMIC, NULL, goproStatusBatteryText},
    {"REC", OME_Label | DYNAMIC, NULL, goproStatusRecordingText},
    {"LINK", OME_Label | DYNAMIC, NULL, goproStatusLinkText},
    {"BACK", OME_Back, NULL, NULL},
    {NULL, OME_END, NULL, NULL}
};

static CMS_Menu cmsx_menuGoproStatus = {
#ifdef CMS_MENU_DEBUG
    .GUARD_text = "MGOPROSTS",
    .GUARD_type = OME_MENU,
#endif
    .onEnter = cmsx_menuGoproStatusOnEnter,
    .onExit = NULL,
    .onDisplayUpdate = cmsx_menuGoproStatusOnDisplayUpdate,
    .entries = cmsx_menuGoproStatusEntries
};

static const OSD_Entry cmsx_menuGoproEntries[] =
{
    {"--- GOPRO ---", OME_Label, NULL, NULL},
    {"CONNECT", OME_Funcall, cmsx_menuGoproSendCommand, (void *)"option=0&setting=0"},
    {"RECORD", OME_TAB, cmsx_menuGoproSetRecord, &goproCmsEntRecord},
    {"RESOLUTION", OME_TAB, cmsx_menuGoproSetResolution, &goproCmsEntResolution},
    {"FPS", OME_TAB, cmsx_menuGoproSetFps, &goproCmsEntFps},
    {"LENS", OME_TAB, cmsx_menuGoproSetLens, &goproCmsEntLens},
    {"HYPERSMOOTH", OME_TAB, cmsx_menuGoproSetHypersmooth, &goproCmsEntHypersmooth},
    {"STATUS", OME_Submenu, cmsMenuChange, &cmsx_menuGoproStatus},
    {"BACK", OME_Back, NULL, NULL},
    {NULL, OME_END, NULL, NULL}
};

CMS_Menu cmsx_menuGopro = {
#ifdef CMS_MENU_DEBUG
    .GUARD_text = "MENUGOPRO",
    .GUARD_type = OME_MENU,
#endif
    .onEnter = cmsx_menuGoproOnEnter,
    .onExit = NULL,
    .onDisplayUpdate = NULL,
    .entries = cmsx_menuGoproEntries
};

#endif