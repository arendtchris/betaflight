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

#include "gopro/gopro_camera.h"

static const char * const hero8RecordLabels[] = { "STOP", "START" };
static const uint16_t hero8RecordValues[] = { 0, 1 };
static const char * const hero8ResolutionLabels[] = { "4K", "2.7K", "1080" };
static const uint16_t hero8ResolutionValues[] = { 1, 4, 9 };
static const char * const hero8FpsLabels[] = { "60", "50", "30", "24" };
static const uint16_t hero8FpsValues[] = { 5, 6, 8, 10 };
static const char * const hero8LensLabels[] = { "WIDE", "LINEAR", "SUPERVIEW" };
static const uint16_t hero8LensValues[] = { 0, 4, 3 };

static goproCmsSettingTable_t hero8Settings[] = {
    {
        .settingId = GOPRO_SETTING_RECORD,
        .commandId = GOPRO_SETTING_RECORD_ID,
        .defaultIndex = 0,
        .currentIndex = 0,
        .tab = { .max = ARRAYLEN(hero8RecordValues) - 1, .names = hero8RecordLabels },
        .callback = NULL,
        .displayLabel = "RECORD",
        .labels = hero8RecordLabels,
        .values = hero8RecordValues,
        .optionCount = (uint8_t)ARRAYLEN(hero8RecordValues)
    },
    {
        .settingId = GOPRO_SETTING_RESOLUTION,
        .commandId = GOPRO_SETTING_RESOLUTION_ID,
        .defaultIndex = 0,
        .currentIndex = 0,
        .tab = { .max = ARRAYLEN(hero8ResolutionValues) - 1, .names = hero8ResolutionLabels },
        .callback = NULL,
        .displayLabel = "RESOLUTION",
        .labels = hero8ResolutionLabels,
        .values = hero8ResolutionValues,
        .optionCount = (uint8_t)ARRAYLEN(hero8ResolutionValues)
    },
    {
        .settingId = GOPRO_SETTING_FPS,
        .commandId = GOPRO_SETTING_FPS_ID,
        .defaultIndex = 2,
        .currentIndex = 2,
        .tab = { .max = ARRAYLEN(hero8FpsValues) - 1, .names = hero8FpsLabels },
        .callback = NULL,
        .displayLabel = "FPS",
        .labels = hero8FpsLabels,
        .values = hero8FpsValues,
        .optionCount = (uint8_t)ARRAYLEN(hero8FpsValues)
    },
    {
        .settingId = GOPRO_SETTING_LENS,
        .commandId = GOPRO_SETTING_LENS_ID,
        .defaultIndex = 0,
        .currentIndex = 0,
        .tab = { .max = ARRAYLEN(hero8LensValues) - 1, .names = hero8LensLabels },
        .callback = NULL,
        .displayLabel = "LENS",
        .labels = hero8LensLabels,
        .values = hero8LensValues,
        .optionCount = (uint8_t)ARRAYLEN(hero8LensValues)
    }
};

const goproCamera_t goproCameraHero8 = {
    .model = GOPRO_CAMERA_MODEL_HERO8,
    .settings = hero8Settings,
    .settingCount = (uint8_t)ARRAYLEN(hero8Settings),
};

#endif
