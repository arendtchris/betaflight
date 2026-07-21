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
#define GOPRO_STATUS_LINK_TEXT_SIZE 16

#define GOPRO_SETTING_RECORD 8
#define GOPRO_SETTING_RESOLUTION 2
#define GOPRO_SETTING_FPS 3
#define GOPRO_SETTING_TIMELAPSE_RATE 5
#define GOPRO_SETTING_WEBCAM_LENS 43
#define GOPRO_SETTING_AUTO_POWER_DOWN 59
#define GOPRO_SETTING_LED 91
#define GOPRO_SETTING_ASPECT_RATIO 108
#define GOPRO_SETTING_LENS 121
#define GOPRO_SETTING_ANTI_FLICKER 134
#define GOPRO_SETTING_HYPERSMOOTH 135
#define GOPRO_SETTING_WIRELESS_BAND 178
#define GOPRO_SETTING_STAR_TRAILS_LENGTH 179
#define GOPRO_SETTING_BITRATE 182

static const char * const goproRecordNames[] = {
    "STOP", "START"
};

static const uint16_t goproRecordOptions[] = {
    0, 1
};

static const char * const goproResolutionNames[] = {
    "8K", "8K 4:3", "4K", "4K 4:3", "4K 9:16", "1440 4:3", "1080", "1080 9:16"
};

static const uint16_t goproResolutionOptions[] = {
    31, 40, 1, 112, 109, 44, 9, 110
};

static const char * const goproFpsNames[] = {
    "960", "800", "480", "400", "240", "200", "120", "100", "60 ", "50 ", "30 ", "25 ", "24 "
};

static const uint16_t goproFpsOptions[] = {
    19, 20, 18, 15, 0, 13, 1, 2, 5, 6, 8, 9, 10
};

static const char * const goproTimelapseRateNames[] = {
    "0.5S", "1S", "2S", "5S", "10S", "30S", "60S", "2M", "5M", "30M", "60M", "OPT11", "OPT12", "OPT13", "OPT14", "OPT15"
};

static const uint16_t goproTimelapseRateOptions[] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15
};

static const char * const goproWebcamLensNames[] = {
    "WIDE", "NARROW", "SUPERVIEW", "LINEAR"
};

static const uint16_t goproWebcamLensOptions[] = {
    0, 2, 3, 4
};

static const char * const goproAutoPowerDownNames[] = {
    "NEVER", "1MIN", "5MIN", "OPT6", "OPT7"
};

static const uint16_t goproAutoPowerDownOptions[] = {
    0, 1, 4, 6, 7
};

static const char * const goproLedNames[] = {
    "MODE 3", "MODE 100"
};

static const uint16_t goproLedOptions[] = {
    3, 100
};

static const char * const goproAspectRatioNames[] = {
    "4:3", "16:9", "9:16"
};

static const uint16_t goproAspectRatioOptions[] = {
    0, 1, 4
};

static const char * const goproLensNames[] = {
    "WIDE", "SUPERVIEW", "LINEAR", "LIN LOCK"
};

static const uint16_t goproLensOptions[] = {
    0, 3, 4, 10
};

static const char * const goproAntiFlickerNames[] = {
    "NTSC", "PAL"
};

static const uint16_t goproAntiFlickerOptions[] = {
    0, 1
};

static const char * const goproHypersmoothNames[] = {
    "OFF", "LOW", "AUTO"
};

static const uint16_t goproHypersmoothOptions[] = {
    0, 1, 4
};

static const char * const goproWirelessBandNames[] = {
    "2.4GHZ", "5GHZ"
};

static const uint16_t goproWirelessBandOptions[] = {
    0, 1
};

static const char * const goproStarTrailsLengthNames[] = {
    "SHORT", "LONG", "MAX"
};

static const uint16_t goproStarTrailsLengthOptions[] = {
    1, 2, 3
};

static const char * const goproBitrateNames[] = {
    "STD", "HIGH", "MAX"
};

static const uint16_t goproBitrateOptions[] = {
    0, 1, 2
};

static char goproStatusBatteryText[GOPRO_STATUS_BATTERY_TEXT_SIZE];
static char goproStatusLinkText[GOPRO_STATUS_LINK_TEXT_SIZE];
static uint8_t goproRecordIndex = 0;
static uint8_t goproResolutionIndex = 2;
static uint8_t goproFpsIndex = 8;
static uint8_t goproTimelapseRateIndex = 0;
static uint8_t goproWebcamLensIndex = 0;
static uint8_t goproAutoPowerDownIndex = 0;
static uint8_t goproLedIndex = 0;
static uint8_t goproAspectRatioIndex = 1;
static uint8_t goproLensIndex = 0;
static uint8_t goproAntiFlickerIndex = 0;
static uint8_t goproHypersmoothIndex = 0;
static uint8_t goproWirelessBandIndex = 0;
static uint8_t goproStarTrailsLengthIndex = 0;
static uint8_t goproBitrateIndex = 0;

static OSD_TAB_t goproCmsEntRecord = { &goproRecordIndex, ARRAYLEN(goproRecordNames) - 1, goproRecordNames };
static OSD_TAB_t goproCmsEntResolution = { &goproResolutionIndex, ARRAYLEN(goproResolutionNames) - 1, goproResolutionNames };
static OSD_TAB_t goproCmsEntFps = { &goproFpsIndex, ARRAYLEN(goproFpsNames) - 1, goproFpsNames };
static OSD_TAB_t goproCmsEntTimelapseRate = { &goproTimelapseRateIndex, ARRAYLEN(goproTimelapseRateNames) - 1, goproTimelapseRateNames };
static OSD_TAB_t goproCmsEntWebcamLens = { &goproWebcamLensIndex, ARRAYLEN(goproWebcamLensNames) - 1, goproWebcamLensNames };
static OSD_TAB_t goproCmsEntAutoPowerDown = { &goproAutoPowerDownIndex, ARRAYLEN(goproAutoPowerDownNames) - 1, goproAutoPowerDownNames };
static OSD_TAB_t goproCmsEntLed = { &goproLedIndex, ARRAYLEN(goproLedNames) - 1, goproLedNames };
static OSD_TAB_t goproCmsEntAspectRatio = { &goproAspectRatioIndex, ARRAYLEN(goproAspectRatioNames) - 1, goproAspectRatioNames };
static OSD_TAB_t goproCmsEntLens = { &goproLensIndex, ARRAYLEN(goproLensNames) - 1, goproLensNames };
static OSD_TAB_t goproCmsEntAntiFlicker = { &goproAntiFlickerIndex, ARRAYLEN(goproAntiFlickerNames) - 1, goproAntiFlickerNames };
static OSD_TAB_t goproCmsEntHypersmooth = { &goproHypersmoothIndex, ARRAYLEN(goproHypersmoothNames) - 1, goproHypersmoothNames };
static OSD_TAB_t goproCmsEntWirelessBand = { &goproWirelessBandIndex, ARRAYLEN(goproWirelessBandNames) - 1, goproWirelessBandNames };
static OSD_TAB_t goproCmsEntStarTrailsLength = { &goproStarTrailsLengthIndex, ARRAYLEN(goproStarTrailsLengthNames) - 1, goproStarTrailsLengthNames };
static OSD_TAB_t goproCmsEntBitrate = { &goproBitrateIndex, ARRAYLEN(goproBitrateNames) - 1, goproBitrateNames };

static void cmsx_menuGoproMission1ProRefreshStatus(void);

static const void *cmsx_menuGoproMission1ProSendCommand(displayPort_t *pDisp, const void *ptr)
{
    UNUSED(pDisp);

    const char *commandParam = (const char *)ptr;
    if (commandParam && commandParam[0]) {
        osdGoproStatusSendCommand(commandParam);
    }

    return NULL;
}

static int8_t cmsx_menuGoproMission1ProFindOptionIndex(const uint16_t *options, uint8_t optionCount, uint16_t optionValue)
{
    for (uint8_t index = 0; index < optionCount; index++) {
        if (options[index] == optionValue) {
            return index;
        }
    }

    return -1;
}

static void cmsx_menuGoproMission1ProSyncTabFromStatus(uint16_t settingId, const uint16_t *options, uint8_t optionCount, uint8_t *selectedIndex)
{
    uint16_t optionValue;
    const char *statusText = osdGoproStatusGet();

    if (!selectedIndex || !statusText || !statusText[0]) {
        return;
    }

    if (!goproJsonGetSettingOption(statusText, settingId, &optionValue)) {
        return;
    }

    const int8_t matchedIndex = cmsx_menuGoproMission1ProFindOptionIndex(options, optionCount, optionValue);
    if (matchedIndex >= 0) {
        *selectedIndex = (uint8_t)matchedIndex;
    }
}

static void cmsx_menuGoproMission1ProSyncRecordTabFromStatus(void)
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

    const int8_t matchedIndex = cmsx_menuGoproMission1ProFindOptionIndex(goproRecordOptions, ARRAYLEN(goproRecordOptions), recordingValue);
    if (matchedIndex >= 0) {
        goproRecordIndex = (uint8_t)matchedIndex;
    }
}

static const void *cmsx_menuGoproMission1ProSendIndexedSetting(displayPort_t *pDisp, uint8_t *selectedIndex, const uint16_t *options, uint8_t optionCount, uint16_t settingId)
{
    if (!selectedIndex || !options || !optionCount) {
        return NULL;
    }

    if (*selectedIndex >= optionCount) {
        *selectedIndex = 0;
    }

    char command[32];
    tfp_sprintf(command, "option=%u&setting=%u", options[*selectedIndex], settingId);
    return cmsx_menuGoproMission1ProSendCommand(pDisp, command);
}

static const void *cmsx_menuGoproMission1ProConnect(displayPort_t *pDisp, const void *self)
{
    UNUSED(self);

    return cmsx_menuGoproMission1ProSendCommand(pDisp, "option=0&setting=0");
}

static const void *cmsx_menuGoproMission1ProSetRecord(displayPort_t *pDisp, const void *self)
{
    UNUSED(self);

    return cmsx_menuGoproMission1ProSendIndexedSetting(pDisp, &goproRecordIndex, goproRecordOptions, ARRAYLEN(goproRecordOptions), GOPRO_SETTING_RECORD);
}

static const void *cmsx_menuGoproMission1ProSetResolution(displayPort_t *pDisp, const void *self)
{
    UNUSED(self);

    return cmsx_menuGoproMission1ProSendIndexedSetting(pDisp, &goproResolutionIndex, goproResolutionOptions, ARRAYLEN(goproResolutionOptions), GOPRO_SETTING_RESOLUTION);
}

static const void *cmsx_menuGoproMission1ProSetFps(displayPort_t *pDisp, const void *self)
{
    UNUSED(self);

    return cmsx_menuGoproMission1ProSendIndexedSetting(pDisp, &goproFpsIndex, goproFpsOptions, ARRAYLEN(goproFpsOptions), GOPRO_SETTING_FPS);
}

static const void *cmsx_menuGoproMission1ProSetTimelapseRate(displayPort_t *pDisp, const void *self)
{
    UNUSED(self);

    return cmsx_menuGoproMission1ProSendIndexedSetting(pDisp, &goproTimelapseRateIndex, goproTimelapseRateOptions, ARRAYLEN(goproTimelapseRateOptions), GOPRO_SETTING_TIMELAPSE_RATE);
}

static const void *cmsx_menuGoproMission1ProSetWebcamLens(displayPort_t *pDisp, const void *self)
{
    UNUSED(self);

    return cmsx_menuGoproMission1ProSendIndexedSetting(pDisp, &goproWebcamLensIndex, goproWebcamLensOptions, ARRAYLEN(goproWebcamLensOptions), GOPRO_SETTING_WEBCAM_LENS);
}

static const void *cmsx_menuGoproMission1ProSetAutoPowerDown(displayPort_t *pDisp, const void *self)
{
    UNUSED(self);

    return cmsx_menuGoproMission1ProSendIndexedSetting(pDisp, &goproAutoPowerDownIndex, goproAutoPowerDownOptions, ARRAYLEN(goproAutoPowerDownOptions), GOPRO_SETTING_AUTO_POWER_DOWN);
}

static const void *cmsx_menuGoproMission1ProSetLed(displayPort_t *pDisp, const void *self)
{
    UNUSED(self);

    return cmsx_menuGoproMission1ProSendIndexedSetting(pDisp, &goproLedIndex, goproLedOptions, ARRAYLEN(goproLedOptions), GOPRO_SETTING_LED);
}

static const void *cmsx_menuGoproMission1ProSetAspectRatio(displayPort_t *pDisp, const void *self)
{
    UNUSED(self);

    return cmsx_menuGoproMission1ProSendIndexedSetting(pDisp, &goproAspectRatioIndex, goproAspectRatioOptions, ARRAYLEN(goproAspectRatioOptions), GOPRO_SETTING_ASPECT_RATIO);
}

static const void *cmsx_menuGoproMission1ProSetLens(displayPort_t *pDisp, const void *self)
{
    UNUSED(self);

    return cmsx_menuGoproMission1ProSendIndexedSetting(pDisp, &goproLensIndex, goproLensOptions, ARRAYLEN(goproLensOptions), GOPRO_SETTING_LENS);
}

static const void *cmsx_menuGoproMission1ProSetAntiFlicker(displayPort_t *pDisp, const void *self)
{
    UNUSED(self);

    return cmsx_menuGoproMission1ProSendIndexedSetting(pDisp, &goproAntiFlickerIndex, goproAntiFlickerOptions, ARRAYLEN(goproAntiFlickerOptions), GOPRO_SETTING_ANTI_FLICKER);
}

static const void *cmsx_menuGoproMission1ProSetHypersmooth(displayPort_t *pDisp, const void *self)
{
    UNUSED(self);

    return cmsx_menuGoproMission1ProSendIndexedSetting(pDisp, &goproHypersmoothIndex, goproHypersmoothOptions, ARRAYLEN(goproHypersmoothOptions), GOPRO_SETTING_HYPERSMOOTH);
}

static const void *cmsx_menuGoproMission1ProSetWirelessBand(displayPort_t *pDisp, const void *self)
{
    UNUSED(self);

    return cmsx_menuGoproMission1ProSendIndexedSetting(pDisp, &goproWirelessBandIndex, goproWirelessBandOptions, ARRAYLEN(goproWirelessBandOptions), GOPRO_SETTING_WIRELESS_BAND);
}

static const void *cmsx_menuGoproMission1ProSetStarTrailsLength(displayPort_t *pDisp, const void *self)
{
    UNUSED(self);

    return cmsx_menuGoproMission1ProSendIndexedSetting(pDisp, &goproStarTrailsLengthIndex, goproStarTrailsLengthOptions, ARRAYLEN(goproStarTrailsLengthOptions), GOPRO_SETTING_STAR_TRAILS_LENGTH);
}

static const void *cmsx_menuGoproMission1ProSetBitrate(displayPort_t *pDisp, const void *self)
{
    UNUSED(self);

    return cmsx_menuGoproMission1ProSendIndexedSetting(pDisp, &goproBitrateIndex, goproBitrateOptions, ARRAYLEN(goproBitrateOptions), GOPRO_SETTING_BITRATE);
}

static const void *cmsx_menuGoproMission1ProOnEnter(displayPort_t *pDisp)
{
    UNUSED(pDisp);

    cmsx_menuGoproMission1ProSyncRecordTabFromStatus();
    cmsx_menuGoproMission1ProSyncTabFromStatus(GOPRO_SETTING_RESOLUTION, goproResolutionOptions, ARRAYLEN(goproResolutionOptions), &goproResolutionIndex);
    cmsx_menuGoproMission1ProSyncTabFromStatus(GOPRO_SETTING_FPS, goproFpsOptions, ARRAYLEN(goproFpsOptions), &goproFpsIndex);
    cmsx_menuGoproMission1ProSyncTabFromStatus(GOPRO_SETTING_TIMELAPSE_RATE, goproTimelapseRateOptions, ARRAYLEN(goproTimelapseRateOptions), &goproTimelapseRateIndex);
    cmsx_menuGoproMission1ProSyncTabFromStatus(GOPRO_SETTING_WEBCAM_LENS, goproWebcamLensOptions, ARRAYLEN(goproWebcamLensOptions), &goproWebcamLensIndex);
    cmsx_menuGoproMission1ProSyncTabFromStatus(GOPRO_SETTING_AUTO_POWER_DOWN, goproAutoPowerDownOptions, ARRAYLEN(goproAutoPowerDownOptions), &goproAutoPowerDownIndex);
    cmsx_menuGoproMission1ProSyncTabFromStatus(GOPRO_SETTING_LED, goproLedOptions, ARRAYLEN(goproLedOptions), &goproLedIndex);
    cmsx_menuGoproMission1ProSyncTabFromStatus(GOPRO_SETTING_ASPECT_RATIO, goproAspectRatioOptions, ARRAYLEN(goproAspectRatioOptions), &goproAspectRatioIndex);
    cmsx_menuGoproMission1ProSyncTabFromStatus(GOPRO_SETTING_LENS, goproLensOptions, ARRAYLEN(goproLensOptions), &goproLensIndex);
    cmsx_menuGoproMission1ProSyncTabFromStatus(GOPRO_SETTING_ANTI_FLICKER, goproAntiFlickerOptions, ARRAYLEN(goproAntiFlickerOptions), &goproAntiFlickerIndex);
    cmsx_menuGoproMission1ProSyncTabFromStatus(GOPRO_SETTING_HYPERSMOOTH, goproHypersmoothOptions, ARRAYLEN(goproHypersmoothOptions), &goproHypersmoothIndex);
    cmsx_menuGoproMission1ProSyncTabFromStatus(GOPRO_SETTING_WIRELESS_BAND, goproWirelessBandOptions, ARRAYLEN(goproWirelessBandOptions), &goproWirelessBandIndex);
    cmsx_menuGoproMission1ProSyncTabFromStatus(GOPRO_SETTING_STAR_TRAILS_LENGTH, goproStarTrailsLengthOptions, ARRAYLEN(goproStarTrailsLengthOptions), &goproStarTrailsLengthIndex);
    cmsx_menuGoproMission1ProSyncTabFromStatus(GOPRO_SETTING_BITRATE, goproBitrateOptions, ARRAYLEN(goproBitrateOptions), &goproBitrateIndex);
    cmsx_menuGoproMission1ProRefreshStatus();

    return NULL;
}

static void cmsx_menuGoproMission1ProCopyStatusText(char *dst, size_t dstSize, const char *src, const char *fallback)
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

static void cmsx_menuGoproMission1ProRefreshStatus(void)
{
    cmsx_menuGoproMission1ProCopyStatusText(goproStatusBatteryText, sizeof(goproStatusBatteryText), osdGoproStatusGetBattery(), "--");
    cmsx_menuGoproMission1ProCopyStatusText(goproStatusLinkText, sizeof(goproStatusLinkText), osdGoproStatusGet()[0] ? "ONLINE" : NULL, "WAITING");
}

static const void *cmsx_menuGoproMission1ProOnDisplayUpdate(displayPort_t *pDisp, const OSD_Entry *selected)
{
    UNUSED(pDisp);
    UNUSED(selected);

    cmsx_menuGoproMission1ProRefreshStatus();

    return NULL;
}

static const OSD_Entry cmsx_menuGoproMission1ProEntries[] =
{
    {"---GOPRO M1 PRO---", OME_Label, NULL, NULL},
    {" LINK", OME_Label | DYNAMIC, NULL, goproStatusLinkText},
    {" BATTERY", OME_Label | DYNAMIC, NULL, goproStatusBatteryText},
    {"CONNECT", OME_Funcall, cmsx_menuGoproMission1ProConnect, NULL},
    {"RECORD", OME_TAB, cmsx_menuGoproMission1ProSetRecord, &goproCmsEntRecord},
    {"RESOLUTION", OME_TAB, cmsx_menuGoproMission1ProSetResolution, &goproCmsEntResolution},
    {"FPS", OME_TAB, cmsx_menuGoproMission1ProSetFps, &goproCmsEntFps},
    {"TIMELAPSE", OME_TAB, cmsx_menuGoproMission1ProSetTimelapseRate, &goproCmsEntTimelapseRate},
    {"WEBCAM LENS", OME_TAB, cmsx_menuGoproMission1ProSetWebcamLens, &goproCmsEntWebcamLens},
    {"AUTO POWER", OME_TAB, cmsx_menuGoproMission1ProSetAutoPowerDown, &goproCmsEntAutoPowerDown},
    {"LED", OME_TAB, cmsx_menuGoproMission1ProSetLed, &goproCmsEntLed},
    {"ASPECT", OME_TAB, cmsx_menuGoproMission1ProSetAspectRatio, &goproCmsEntAspectRatio},
    {"LENS", OME_TAB, cmsx_menuGoproMission1ProSetLens, &goproCmsEntLens},
    {"ANTI FLICKER", OME_TAB, cmsx_menuGoproMission1ProSetAntiFlicker, &goproCmsEntAntiFlicker},
    {"HYPERSMOOTH", OME_TAB, cmsx_menuGoproMission1ProSetHypersmooth, &goproCmsEntHypersmooth},
    {"WIFI BAND", OME_TAB, cmsx_menuGoproMission1ProSetWirelessBand, &goproCmsEntWirelessBand},
    {"STAR TRAILS", OME_TAB, cmsx_menuGoproMission1ProSetStarTrailsLength, &goproCmsEntStarTrailsLength},
    {"BITRATE", OME_TAB, cmsx_menuGoproMission1ProSetBitrate, &goproCmsEntBitrate},
    {"BACK", OME_Back, NULL, NULL},
    {NULL, OME_END, NULL, NULL}
};

CMS_Menu cmsx_menuGoproMission1Pro = {
#ifdef CMS_MENU_DEBUG
    .GUARD_text = "MENUGPM1PRO",
    .GUARD_type = OME_MENU,
#endif
    .onEnter = cmsx_menuGoproMission1ProOnEnter,
    .onExit = NULL,
    .onDisplayUpdate = cmsx_menuGoproMission1ProOnDisplayUpdate,
    .entries = cmsx_menuGoproMission1ProEntries
};

#endif
