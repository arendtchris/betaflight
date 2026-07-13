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
#include <string.h>

#include "common/printf.h"
#include "common/gopro_json.h"

#include "cms/cms.h"
#include "cms/cms_types.h"
#include "cms/cms_menu_gopro.h"
#include "osd/osd_gopro_status.h"

#define GOPRO_SETTING_FPS 3
#define GOPRO_SETTING_HYPERSMOOTH 135

static uint16_t goproPendingSettingId = 0;
static uint16_t goproCurrentFpsOption = 0xFFFF;
static uint16_t goproCurrentHypersmoothOption = 0xFFFF;

static char goproCurrentFpsText[14] = " ?";
static char goproCurrentHypersmoothText[14] = " ?";

static const char *cmsx_menuGoproGetFpsLabelByOption(uint16_t option)
{
    switch (option) {
    case 0:  return "240.0";
    case 1:  return "120.0";
    case 2:  return "100.0";
    case 3:  return "90.0";
    case 5:  return "60.0";
    case 6:  return "50.0";
    case 8:  return "30.0";
    case 9:  return "25.0";
    case 10: return "24.0";
    case 13: return "200.0";
    case 15: return "400.0";
    case 16: return "360.0";
    case 17: return "300.0";
    case 18: return "480.0";
    case 19: return "960.0";
    case 20: return "800.0";
    default: return NULL;
    }
}

static const char *cmsx_menuGoproGetHypersmoothLabelByOption(uint16_t option)
{
    switch (option) {
    case 0: return "OFF";
    case 1: return "LOW";
    case 4: return "AUTO";
    default: return NULL;
    }
}

static bool cmsx_menuGoproParseCurrentOptions(const char *json, uint16_t *fpsOption, uint16_t *hypersmoothOption)
{
    bool found = false;
    uint16_t value;
    if (fpsOption && goproJsonGetSettingOption(json, GOPRO_SETTING_FPS, &value)) {
        *fpsOption = value;
        found = true;
    }
    if (hypersmoothOption && goproJsonGetSettingOption(json, GOPRO_SETTING_HYPERSMOOTH, &value)) {
        *hypersmoothOption = value;
        found = true;
    }

    return found;
}

static void cmsx_menuGoproUpdateCurrentValueStrings(void)
{
    const char *fpsLabel = cmsx_menuGoproGetFpsLabelByOption(goproCurrentFpsOption);
    const char *hypersmoothLabel = cmsx_menuGoproGetHypersmoothLabelByOption(goproCurrentHypersmoothOption);

    if (fpsLabel) {
        tfp_sprintf(goproCurrentFpsText, "%s", fpsLabel);
    } else if (goproCurrentFpsOption != 0xFFFFU) {
        tfp_sprintf(goproCurrentFpsText, "OPT%u", goproCurrentFpsOption);
    } else {
        tfp_sprintf(goproCurrentFpsText, "?");
    }

    if (hypersmoothLabel) {
        tfp_sprintf(goproCurrentHypersmoothText, "%s", hypersmoothLabel);
    } else if (goproCurrentHypersmoothOption != 0xFFFFU) {
        tfp_sprintf(goproCurrentHypersmoothText, "OPT%u", goproCurrentHypersmoothOption);
    } else {
        tfp_sprintf(goproCurrentHypersmoothText, "?");
    }
}

static const void *cmsx_menuGoproOnDisplayUpdate(displayPort_t *pDisp, const OSD_Entry *selected)
{
    UNUSED(pDisp);
    UNUSED(selected);

    const char *response = osdGoproStatusGet();
    uint16_t option;

    if (response && response[0]) {
        if (cmsx_menuGoproParseCurrentOptions(response, &goproCurrentFpsOption, &goproCurrentHypersmoothOption)) {
            goproPendingSettingId = 0;
        } else if (goproJsonExtractUint16(response, response + strlen(response), "option", &option)) {
            if (goproPendingSettingId == GOPRO_SETTING_FPS) {
                goproCurrentFpsOption = option;
            } else if (goproPendingSettingId == GOPRO_SETTING_HYPERSMOOTH) {
                goproCurrentHypersmoothOption = option;
            }
        }
    }

    cmsx_menuGoproUpdateCurrentValueStrings();
    return NULL;
}

static const void *cmsx_menuGoproReadCurrentSetting(displayPort_t *pDisp, const void *ptr)
{
    UNUSED(pDisp);

    const char *settingParam = (const char *)ptr;
    if (!settingParam || strncmp(settingParam, "setting=", 8) != 0) {
        return NULL;
    }

    uint16_t settingId;
    if (!goproJsonParseUint16(settingParam + 8, &settingId)) {
        return NULL;
    }

    goproPendingSettingId = settingId;

    {
        static const char commandPrefix[] = "GET /gopro/camera/setting?";
        char command[64];
        size_t prefixLength = strlen(commandPrefix);
        size_t paramLength = strlen(settingParam);

        if (prefixLength + paramLength >= sizeof(command)) {
            return NULL;
        }

        memcpy(command, commandPrefix, prefixLength);
        memcpy(command + prefixLength, settingParam, paramLength);
        command[prefixLength + paramLength] = '\0';

        osdGoproStatusSendCommand(command);
    }

    return NULL;
}

static const void *cmsx_menuGoproReadCurrentState(displayPort_t *pDisp, const void *ptr)
{
    UNUSED(pDisp);
    UNUSED(ptr);

    goproPendingSettingId = 0;
    osdGoproStatusSendCommand("GET /gopro/camera/state");
    return NULL;
}

static const void *cmsx_menuGoproSendCommand(displayPort_t *pDisp, const void *ptr)
{
    UNUSED(pDisp);

    const char *commandParam = (const char *)ptr;
    if (commandParam && commandParam[0]) {
        static const char commandPrefix[] = "GET /gopro/camera/setting?";
        char command[64];
        size_t length = 0;
        const size_t maxLength = sizeof(command) - 1;

        const size_t prefixLength = strlen(commandPrefix);
        const size_t paramLength = strlen(commandParam);
        if (prefixLength + paramLength > maxLength) {
            return NULL;
        }

        memcpy(command + length, commandPrefix, prefixLength);
        length += prefixLength;
        memcpy(command + length, commandParam, paramLength);
        length += paramLength;
        command[length] = '\0';

        osdGoproStatusSendCommand(command);
    }

    return NULL;
}

static const OSD_Entry cmsx_menuGoproEntries[] =
{
    {"--- GOPRO FPS ---", OME_Label, NULL, NULL},
    {"CUR", OME_Label | DYNAMIC, NULL, goproCurrentFpsText},
    {"READ CUR", OME_Funcall, cmsx_menuGoproReadCurrentSetting, (void *)"setting=3"},
    {"240.0", OME_Funcall, cmsx_menuGoproSendCommand, (void *)"option=0&setting=3"},
    {"120.0", OME_Funcall, cmsx_menuGoproSendCommand, (void *)"option=1&setting=3"},
    {"100.0", OME_Funcall, cmsx_menuGoproSendCommand, (void *)"option=2&setting=3"},
    {"90.0",  OME_Funcall, cmsx_menuGoproSendCommand, (void *)"option=3&setting=3"},
    {"60.0",  OME_Funcall, cmsx_menuGoproSendCommand, (void *)"option=5&setting=3"},
    {"50.0",  OME_Funcall, cmsx_menuGoproSendCommand, (void *)"option=6&setting=3"},
    {"30.0",  OME_Funcall, cmsx_menuGoproSendCommand, (void *)"option=8&setting=3"},
    {"25.0",  OME_Funcall, cmsx_menuGoproSendCommand, (void *)"option=9&setting=3"},
    {"24.0",  OME_Funcall, cmsx_menuGoproSendCommand, (void *)"option=10&setting=3"},
    {"200.0", OME_Funcall, cmsx_menuGoproSendCommand, (void *)"option=13&setting=3"},
    {"400.0", OME_Funcall, cmsx_menuGoproSendCommand, (void *)"option=15&setting=3"},
    {"360.0", OME_Funcall, cmsx_menuGoproSendCommand, (void *)"option=16&setting=3"},
    {"300.0", OME_Funcall, cmsx_menuGoproSendCommand, (void *)"option=17&setting=3"},
    {"480.0", OME_Funcall, cmsx_menuGoproSendCommand, (void *)"option=18&setting=3"},
    {"960.0", OME_Funcall, cmsx_menuGoproSendCommand, (void *)"option=19&setting=3"},
    {"800.0", OME_Funcall, cmsx_menuGoproSendCommand, (void *)"option=20&setting=3"},
    {"-- HYPERSMOOTH --", OME_Label, NULL, NULL},
    {"CUR", OME_Label | DYNAMIC, NULL, goproCurrentHypersmoothText},
    {"READ CUR", OME_Funcall, cmsx_menuGoproReadCurrentSetting, (void *)"setting=135"},
    {"OFF", OME_Funcall, cmsx_menuGoproSendCommand, (void *)"option=0&setting=135"},
    {"LOW", OME_Funcall, cmsx_menuGoproSendCommand, (void *)"option=1&setting=135"},
    {"AUTO BOOST", OME_Funcall, cmsx_menuGoproSendCommand, (void *)"option=4&setting=135"},
    {"READ STATE", OME_Funcall, cmsx_menuGoproReadCurrentState, NULL},
    {"BACK", OME_Back, NULL, NULL},
    {NULL, OME_END, NULL, NULL}
};

CMS_Menu cmsx_menuGopro = {
#ifdef CMS_MENU_DEBUG
    .GUARD_text = "MENUGOPRO",
    .GUARD_type = OME_MENU,
#endif
    .onEnter = NULL,
    .onExit = NULL,
    .onDisplayUpdate = cmsx_menuGoproOnDisplayUpdate,
    .entries = cmsx_menuGoproEntries
};

#endif