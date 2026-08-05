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

#include "cms/cms_menu_gopro12_settings.h"
#include "cms/cms_types.h"
#include "osd/osd_gopro_status.h"

static const char * const gopro12RecordLabels[] = {
    "STOP", "START"
};

static const uint16_t gopro12RecordValues[] = {
    0, 1
};

static const char * const gopro12ResolutionLabels[] = {
    "5.3K", "5.3K 8:7", "5.3K 4:3", "4K", "4K 8:7", "4K 4:3", "2.7K", "2.7K 4:3", "1080"
};

static const uint16_t gopro12ResolutionValues[] = {
    100, 26, 27, 1, 28, 18, 4, 6, 9
};

static const char * const gopro12FpsLabels[] = {
    "240.0", "200.0", "120.0", "100.0", "60.0", "50.0", "30.0", "25.0", "24.0"
};

static const uint16_t gopro12FpsValues[] = {
    0, 13, 1, 2, 5, 6, 8, 9, 10
};

static const char * const gopro12LensLabels[] = {
    "Wide", "Superview", "Linear", "Max SuperView", "Linear + Horizon Leveling", "HyperView", "Linear + Horizon Lock"
};

static const uint16_t gopro12LensValues[] = {
    0, 3, 4, 7, 8, 9, 10
};

static const char * const gopro12HypersmoothLabels[] = {
    "Off", "Low", "Boost", "Auto Boost"
};

static const uint16_t gopro12HypersmoothValues[] = {
    0, 1, 3, 4
};

// Shared GoPro CMS settings table for the menu entries.
static goproCmsSettingTable_t gopro12Settings[] = {
    {
        .settingId = GOPRO_SETTING_RECORD,
        .commandId = GOPRO_SETTING_RECORD_ID,
        .defaultIndex = 0,
        .currentIndex = 0,
        .tab = {
            .max = ARRAYLEN(gopro12RecordValues) - 1,
            .names = gopro12RecordLabels,
        },
        .callback = NULL,
        .displayLabel = "RECORD",
        .labels = gopro12RecordLabels,
        .values = gopro12RecordValues,
        .optionCount = (uint8_t)ARRAYLEN(gopro12RecordValues)
    },
    {
        .settingId = GOPRO_SETTING_RESOLUTION,
        .commandId = GOPRO_SETTING_RESOLUTION_ID,
        .defaultIndex = 2,
        .currentIndex = 2,
        .tab = {
            .max = ARRAYLEN(gopro12ResolutionValues) - 1,
            .names = gopro12ResolutionLabels,
        },
        .callback = NULL,
        .displayLabel = "RESOLUTION",
        .labels = gopro12ResolutionLabels,
        .values = gopro12ResolutionValues,
        .optionCount = (uint8_t)ARRAYLEN(gopro12ResolutionValues)
    },
    {
        .settingId = GOPRO_SETTING_FPS,
        .commandId = GOPRO_SETTING_FPS_ID,
        .defaultIndex = 4,
        .currentIndex = 4,
        .tab = {
            .max = ARRAYLEN(gopro12FpsValues) - 1,
            .names = gopro12FpsLabels,
        },
        .callback = NULL,
        .displayLabel = "FPS",
        .labels = gopro12FpsLabels,
        .values = gopro12FpsValues,
        .optionCount = (uint8_t)ARRAYLEN(gopro12FpsValues)
    },
    {
        .settingId = GOPRO_SETTING_LENS,
        .commandId = GOPRO_SETTING_LENS_ID,
        .defaultIndex = 1,
        .currentIndex = 1,
        .tab = {
            .max = ARRAYLEN(gopro12LensValues) - 1,
            .names = gopro12LensLabels,
        },
        .callback = NULL,
        .displayLabel = "LENS",
        .labels = gopro12LensLabels,
        .values = gopro12LensValues,
        .optionCount = (uint8_t)ARRAYLEN(gopro12LensValues)
    },
    {
        .settingId = GOPRO_SETTING_HYPERSMOOTH,
        .commandId = GOPRO_SETTING_HYPERSMOOTH_ID,
        .defaultIndex = 0,
        .currentIndex = 0,
        .tab = {
            .max = ARRAYLEN(gopro12HypersmoothValues) - 1,
            .names = gopro12HypersmoothLabels,
        },
        .callback = NULL,
        .displayLabel = "HYPERSMOOTH",
        .labels = gopro12HypersmoothLabels,
        .values = gopro12HypersmoothValues,
        .optionCount = (uint8_t)ARRAYLEN(gopro12HypersmoothValues)
    }
};

// Returns the shared GoPro settings table used by the CMS menu.
goproCmsSettingTable_t *cmsMenuGopro12GetSettings(void)
{
    return gopro12Settings;
}

// Returns the number of GoPro settings entries in the shared table.
uint8_t cmsMenuGopro12GetSettingsCount(void)
{
    return (uint8_t)ARRAYLEN(gopro12Settings);
}

// Looks up a single GoPro setting by its identifier.
goproCmsSettingTable_t *cmsMenuGopro12GetSetting(goproCmsSettingId_e settingId)
{
    for (uint8_t index = 0; index < ARRAYLEN(gopro12Settings); index++) {
        if (gopro12Settings[index].settingId == settingId) {
            return &gopro12Settings[index];
        }
    }

    return NULL;
}

#endif
