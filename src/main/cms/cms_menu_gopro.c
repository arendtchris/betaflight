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
#include "cms/cms_menu_gopro_settings.h"
#include "osd/osd_gopro_status.h"

// Buffer sizes for the dynamic status text shown in the GoPro CMS menu.
#define GOPRO_STATUS_BATTERY_TEXT_SIZE 8
#define GOPRO_STATUS_LINK_TEXT_SIZE 16

// Fixed command used for the CONNECT action in the GoPro menu.
#define GOPRO_COMMAND_CONNECT 0
#define GOPRO_MENU_ENTRY_COUNT 11

// Dynamic status text for the menu entries that display battery and link state.
static char goproStatusBatteryText[GOPRO_STATUS_BATTERY_TEXT_SIZE];
static char goproStatusLinkText[GOPRO_STATUS_LINK_TEXT_SIZE];

static OSD_Entry cmsx_menuGoproEntries[GOPRO_MENU_ENTRY_COUNT];
static void cmsx_menuGoproRefreshStatus(void);
static void cmsx_menuGoproUpdateTabData(goproCmsSettingTable_t *setting);

// Sends a raw GoPro command over the existing status/control UART link.
static const void *cmsx_menuGoproSendCommand(uint16_t optionId,  uint16_t settingId)
{
    osdGoproStatusSendCommand(optionId, settingId);
    return NULL;
}

// Sends the currently selected GoPro setting value to the device.
static const void *cmsx_menuGoproSendIndexedSetting(goproCmsSettingTable_t *setting)
{
    if (!setting || !setting->values || !setting->optionCount) {
        return NULL;
    }

    if (setting->currentIndex >= setting->optionCount) {
        setting->currentIndex = 0;
    }

    osdGoproStatusSendCommand(setting->values[setting->currentIndex], setting->commandId);

    return NULL;
}

// Resolves a setting by ID and forwards it to the shared send helper.
const void *cmsx_menuGoproSendSetting(goproCmsSettingId_e settingId)
{
    goproCmsSettingTable_t *setting = cmsMenuGoproGetSetting(settingId);
    return cmsx_menuGoproSendIndexedSetting(setting);
}

// Synchronizes the CMS selection with the current value reported by the GoPro status JSON.
static void cmsx_menuGoproSyncTabFromStatus(goproCmsSettingTable_t *setting)
{
    uint16_t optionValue;
    const char *statusText = osdGoproStatusGet();

    if (!setting || !statusText || !statusText[0]) {
        return;
    }

    if (!goproJsonGetSettingOption(statusText, setting->commandId, &optionValue)) {
        return;
    }

    if (!setting->values || !setting->optionCount) {
        return;
    }

    for (uint8_t index = 0; index < setting->optionCount; index++) {
        if (setting->values[index] == optionValue) {
            setting->currentIndex = (uint8_t)index;
            break;
        }
    }
}

// Sends the fixed CONNECT command for the GoPro without exposing a selectable value.
static const void *cmsx_menuGoproConnect(displayPort_t *pDisp, const void *self)
{
    UNUSED(pDisp);
    UNUSED(self);

    return cmsx_menuGoproSendCommand(GOPRO_COMMAND_CONNECT, 0);
}

// Updates the CMS tab metadata for one GoPro settings entry.
static void cmsx_menuGoproUpdateTabData(goproCmsSettingTable_t *setting)
{
    if (!setting) {
        return;
    }

    setting->tab.val = &setting->currentIndex;
    setting->tab.max = setting->optionCount - 1;
    setting->tab.names = setting->labels;
}

// Initializes the menu state from the latest GoPro status when the CMS menu is entered.
static const void *cmsx_menuGoproOnEnter(displayPort_t *pDisp)
{
    UNUSED(pDisp);

    goproCmsSettingTable_t *settings = cmsMenuGoproGetSettings();
    const uint8_t settingCount = cmsMenuGoproGetSettingsCount();

    for (uint8_t index = 0; index < settingCount; index++) {
        goproCmsSettingTable_t *setting = &settings[index];

        cmsx_menuGoproSyncTabFromStatus(setting);
        cmsx_menuGoproUpdateTabData(setting);
    }

    return NULL;
}

// Copies status text into a fixed-size buffer and falls back to a safe default if needed.
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

// Refreshes the dynamic status strings shown in the GoPro CMS menu.
static void cmsx_menuGoproRefreshStatus(void)
{
    cmsx_menuGoproCopyStatusText(goproStatusBatteryText, sizeof(goproStatusBatteryText), osdGoproStatusGetBattery(), "--");
    // refactor using a helper function 
    cmsx_menuGoproCopyStatusText(goproStatusLinkText, sizeof(goproStatusLinkText), osdGoproStatusGetLink(), "OFFLINE");
}

// Keeps the status strings up to date while the GoPro menu is displayed.
static const void *cmsx_menuGoproStatusOnDisplayUpdate(displayPort_t *pDisp, const OSD_Entry *selected)
{
    UNUSED(pDisp);
    UNUSED(selected);

    cmsx_menuGoproRefreshStatus();

    return NULL;
}

static OSD_Entry cmsx_menuGoproEntries[] =
{
    {"---GOPRO SETTINGS---", OME_Label, NULL, NULL},
    {" LINK", OME_Label | DYNAMIC, NULL, goproStatusLinkText},
    {" BATTERY", OME_Label | DYNAMIC, NULL, goproStatusBatteryText},
    {"CONNECT", OME_Funcall, cmsx_menuGoproConnect, NULL},
    {"RECORD", OME_TAB, NULL, NULL},
    {"RESOLUTION", OME_TAB, NULL, NULL},
    {"FPS", OME_TAB, NULL, NULL},
    {"LENS", OME_TAB, NULL, NULL},
    {"HYPERSMOOTH", OME_TAB, NULL, NULL},
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
    .onDisplayUpdate = cmsx_menuGoproStatusOnDisplayUpdate,
    .entries = cmsx_menuGoproEntries
};

#endif