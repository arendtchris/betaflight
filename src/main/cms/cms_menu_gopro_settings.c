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

#include "platform.h"

#if defined(USE_OSD) && defined(USE_CMS)

#include "cms/cms_menu_gopro_settings.h"
#include "cms/cms_types.h"
#include "osd/osd_gopro_status.h"

#define GOPRO_SETTING_RECORD_ID 8
#define GOPRO_SETTING_RESOLUTION_ID 2
#define GOPRO_SETTING_FPS_ID 3
#define GOPRO_SETTING_LENS_ID 121
#define GOPRO_SETTING_HYPERSMOOTH_ID 135

static const char * const goproRecordLabels[] = {
    "STOP", "START"
};

static const uint16_t goproRecordValues[] = {
    0, 1
};

static const char * const goproResolutionLabels[] = {
    "5.3K", "5.3K 8:7", "5.3K 4:3", "4K", "4K 8:7", "4K 4:3", "2.7K", "2.7K 4:3", "1080"
};

static const uint16_t goproResolutionValues[] = {
    100, 26, 27, 1, 28, 18, 4, 6, 9
};

static const char * const goproFpsLabels[] = {
    "240", "200", "120", "100", "60 ", "50 ", "30 ", "25 ", "24 "
};

static const uint16_t goproFpsValues[] = {
    0, 13, 1, 2, 5, 6, 8, 9, 10
};

static const char * const goproLensLabels[] = {
    "WIDE", "SUPERVIEW", "LINEAR", "MAX SV", "LIN HL", "HYPERVIEW", "LIN LOCK"
};

static const uint16_t goproLensValues[] = {
    0, 3, 4, 7, 8, 9, 10
};

static const char * const goproHypersmoothLabels[] = {
    "OFF", "LOW", "BOOST", "AUTO"
};

static const uint16_t goproHypersmoothValues[] = {
    0, 1, 3, 4
};

// Shared GoPro CMS settings table for the menu entries and their callbacks.
static goproCmsSettingTable_t goproSettings[] = {
    {
        .settingId = GOPRO_SETTING_RECORD,
        .commandId = GOPRO_SETTING_RECORD_ID,
        .defaultIndex = 0,
        .currentIndex = 0,
        .tab = {
            .max = ARRAYLEN(goproRecordValues) - 1,
            .names = goproRecordLabels,
        },
        .callback = cmsMenuGoproRecordCallback,
        .displayLabel = "RECORD",
        .labels = goproRecordLabels,
        .values = goproRecordValues,
        .optionCount = (uint8_t)ARRAYLEN(goproRecordValues)
    },
    {
        .settingId = GOPRO_SETTING_RESOLUTION,
        .commandId = GOPRO_SETTING_RESOLUTION_ID,
        .defaultIndex = 2,
        .currentIndex = 2,
        .tab = {
            .max = ARRAYLEN(goproResolutionValues) - 1,
            .names = goproResolutionLabels,
        },
        .callback = cmsMenuGoproResolutionCallback,
        .displayLabel = "RESOLUTION",
        .labels = goproResolutionLabels,
        .values = goproResolutionValues,
        .optionCount = (uint8_t)ARRAYLEN(goproResolutionValues)
    },
    {
        .settingId = GOPRO_SETTING_FPS,
        .commandId = GOPRO_SETTING_FPS_ID,
        .defaultIndex = 4,
        .currentIndex = 4,
        .tab = {
            .max = ARRAYLEN(goproFpsValues) - 1,
            .names = goproFpsLabels,
        },
        .callback = cmsMenuGoproFpsCallback,
        .displayLabel = "FPS",
        .labels = goproFpsLabels,
        .values = goproFpsValues,
        .optionCount = (uint8_t)ARRAYLEN(goproFpsValues)
    },
    {
        .settingId = GOPRO_SETTING_LENS,
        .commandId = GOPRO_SETTING_LENS_ID,
        .defaultIndex = 1,
        .currentIndex = 1,
        .tab = {
            .max = ARRAYLEN(goproLensValues) - 1,
            .names = goproLensLabels,
        },
        .callback = cmsMenuGoproLensCallback,
        .displayLabel = "LENS",
        .labels = goproLensLabels,
        .values = goproLensValues,
        .optionCount = (uint8_t)ARRAYLEN(goproLensValues)
    },
    {
        .settingId = GOPRO_SETTING_HYPERSMOOTH,
        .commandId = GOPRO_SETTING_HYPERSMOOTH_ID,
        .defaultIndex = 0,
        .currentIndex = 0,
        .tab = {
            .max = ARRAYLEN(goproHypersmoothValues) - 1,
            .names = goproHypersmoothLabels,
        },
        .callback = cmsMenuGoproHypersmoothCallback,
        .displayLabel = "HYPERSMOOTH",
        .labels = goproHypersmoothLabels,
        .values = goproHypersmoothValues,
        .optionCount = (uint8_t)ARRAYLEN(goproHypersmoothValues)
    }
};

// cms Callback functions for each GoPro setting, which send the selected value to the GoPro via the status port.
const void *cmsMenuGoproRecordCallback(displayPort_t *pDisp, const void *self)
{
    UNUSED(pDisp);
    UNUSED(self);
    return cmsx_menuGoproSendSetting(GOPRO_SETTING_RECORD);
}

const void *cmsMenuGoproResolutionCallback(displayPort_t *pDisp, const void *self)
{
    UNUSED(pDisp);
    UNUSED(self);
    return cmsx_menuGoproSendSetting(GOPRO_SETTING_RESOLUTION);
}

const void *cmsMenuGoproFpsCallback(displayPort_t *pDisp, const void *self)
{
    UNUSED(pDisp);
    UNUSED(self);
    return cmsx_menuGoproSendSetting(GOPRO_SETTING_FPS);
}

const void *cmsMenuGoproLensCallback(displayPort_t *pDisp, const void *self)
{
    UNUSED(pDisp);
    UNUSED(self);
    return cmsx_menuGoproSendSetting(GOPRO_SETTING_LENS);
}

const void *cmsMenuGoproHypersmoothCallback(displayPort_t *pDisp, const void *self)
{
    UNUSED(pDisp);
    UNUSED(self);
    return cmsx_menuGoproSendSetting(GOPRO_SETTING_HYPERSMOOTH);
}

// Returns the shared GoPro settings table used by the CMS menu.
goproCmsSettingTable_t *cmsMenuGoproGetSettings(void)
{
    return goproSettings;
}

// Returns the number of GoPro settings entries in the shared table.
uint8_t cmsMenuGoproGetSettingsCount(void)
{
    return (uint8_t)ARRAYLEN(goproSettings);
}

// Looks up a single GoPro setting by its identifier.
goproCmsSettingTable_t *cmsMenuGoproGetSetting(goproCmsSettingId_e settingId)
{
    for (uint8_t index = 0; index < ARRAYLEN(goproSettings); index++) {
        if (goproSettings[index].settingId == settingId) {
            return &goproSettings[index];
        }
    }

    return NULL;
}

#endif
