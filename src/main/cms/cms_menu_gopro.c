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
#include "gopro/gopro_camera.h"
#include "osd/osd_gopro_status.h"

#ifndef container_of
#define container_of(ptr, type, member) ((type *)((char *)(ptr) - offsetof(type, member)))
#endif

// Buffer sizes for the dynamic status text shown in the GoPro CMS menu.

#define GOPRO_STATUS_TEXT_SIZE 16

// Fixed command used for the CONNECT action in the GoPro menu.
#define GOPRO_COMMAND_CONNECT 0
#define GOPRO_MENU_ENTRY_COUNT 32





// Dynamic status text for the menu entries that display battery, link, and name state.
static char goproStatusBatteryText[GOPRO_STATUS_TEXT_SIZE];
static char goproStatusLinkText[GOPRO_STATUS_TEXT_SIZE];
static char goproStatusNameText[GOPRO_STATUS_TEXT_SIZE];

static OSD_Entry cmsx_menuGoproEntries[GOPRO_MENU_ENTRY_COUNT];

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

const void *cmsx_menuGoproGenericCallback(displayPort_t *pDisp, const void *self)
{
    UNUSED(pDisp);

    if (!self) {
        return NULL;
    }

    goproCmsSettingTable_t *setting = container_of((const OSD_TAB_t *)self, goproCmsSettingTable_t, tab);

    if (!setting) {
        return NULL;
    }

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
void cmsx_menuGoproUpdateTabData(goproCmsSettingTable_t *setting)
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

    goproCameraModel_e model = goproCameraDetectModelFromStatus(osdGoproStatusGetName());
   

    const goproCamera_t *camera =  goproCameraGetModelData(model);
    const uint8_t settingCount = camera ? camera->settingCount : 0;
    const uint8_t settingsStartIndex = 4;
    const uint8_t backEntryIndex = settingsStartIndex + settingCount;
    goproCmsSettingTable_t *settings = camera->settings;

    cmsx_menuGoproEntries[0] = (OSD_Entry){" GOPRO", OME_Label | DYNAMIC, NULL, goproStatusNameText};
    cmsx_menuGoproEntries[1] = (OSD_Entry){" LINK", OME_Label | DYNAMIC, NULL, goproStatusLinkText};
    cmsx_menuGoproEntries[2] = (OSD_Entry){" BATTERY", OME_Label | DYNAMIC, NULL, goproStatusBatteryText};
 

    cmsx_menuGoproEntries[3] = (OSD_Entry){"CONNECT", OME_Funcall, cmsx_menuGoproConnect, NULL};

    for (uint8_t index = 0; index < settingCount; index++) {
        goproCmsSettingTable_t *setting = &settings[index];

        cmsx_menuGoproSyncTabFromStatus(setting);
        cmsx_menuGoproUpdateTabData(setting);
        const uint8_t entryIndex = settingsStartIndex + index;
        cmsx_menuGoproEntries[entryIndex].text = setting->displayLabel;
        cmsx_menuGoproEntries[entryIndex].flags = OME_TAB;
        cmsx_menuGoproEntries[entryIndex].func = cmsx_menuGoproGenericCallback;
        cmsx_menuGoproEntries[entryIndex].data = &setting->tab;

    }
    cmsx_menuGoproEntries[backEntryIndex] = (OSD_Entry){"BACK", OME_Back, NULL, NULL};
    cmsx_menuGoproEntries[backEntryIndex + 1] = (OSD_Entry){NULL, OME_END, NULL, NULL};
    
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

 

// Keeps the status strings up to date while the GoPro menu is displayed.
static const void *cmsx_menuGoproStatusOnDisplayUpdate(displayPort_t *pDisp, const OSD_Entry *selected)
{
    UNUSED(pDisp);
    UNUSED(selected);

    cmsx_menuGoproCopyStatusText(goproStatusBatteryText, sizeof(goproStatusBatteryText), osdGoproStatusGetBattery(), "--");
    cmsx_menuGoproCopyStatusText(goproStatusLinkText, sizeof(goproStatusLinkText), osdGoproStatusGetLink(), "OFFLINE");
    cmsx_menuGoproCopyStatusText(goproStatusNameText, sizeof(goproStatusNameText), osdGoproStatusGetName(), "--");

    return NULL;
}


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