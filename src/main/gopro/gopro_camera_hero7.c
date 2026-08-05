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

static const char * const hero7RecordLabels[] = { "STOP", "START" };
static const uint16_t hero7RecordValues[] = { 0, 1 };
static const char * const hero7ResolutionLabels[] = { "4K", "2.7K", "1080" };
static const uint16_t hero7ResolutionValues[] = { 1, 4, 9 };
static const char * const hero7FpsLabels[] = { "60", "50", "30", "24" };
static const uint16_t hero7FpsValues[] = { 5, 6, 8, 10 };
static const char * const hero7LensLabels[] = { "WIDE", "LINEAR", "SUPERVIEW" };
static const uint16_t hero7LensValues[] = { 0, 4, 3 };

static goproCmsSettingTable_t hero7Settings[] = {
    {
        .settingId = GOPRO_SETTING_RECORD,
        .commandId = GOPRO_SETTING_RECORD_ID,
        .defaultIndex = 0,
        .currentIndex = 0,
        .tab = { .max = ARRAYLEN(hero7RecordValues) - 1, .names = hero7RecordLabels },
        .callback = NULL,
        .displayLabel = "RECORD",
        .labels = hero7RecordLabels,
        .values = hero7RecordValues,
        .optionCount = (uint8_t)ARRAYLEN(hero7RecordValues)
    },
    {
        .settingId = GOPRO_SETTING_RESOLUTION,
        .commandId = GOPRO_SETTING_RESOLUTION_ID,
        .defaultIndex = 1,
        .currentIndex = 1,
        .tab = { .max = ARRAYLEN(hero7ResolutionValues) - 1, .names = hero7ResolutionLabels },
        .callback = NULL,
        .displayLabel = "RESOLUTION",
        .labels = hero7ResolutionLabels,
        .values = hero7ResolutionValues,
        .optionCount = (uint8_t)ARRAYLEN(hero7ResolutionValues)
    },
    {
        .settingId = GOPRO_SETTING_FPS,
        .commandId = GOPRO_SETTING_FPS_ID,
        .defaultIndex = 2,
        .currentIndex = 2,
        .tab = { .max = ARRAYLEN(hero7FpsValues) - 1, .names = hero7FpsLabels },
        .callback = NULL,
        .displayLabel = "FPS",
        .labels = hero7FpsLabels,
        .values = hero7FpsValues,
        .optionCount = (uint8_t)ARRAYLEN(hero7FpsValues)
    },
    {
        .settingId = GOPRO_SETTING_LENS,
        .commandId = GOPRO_SETTING_LENS_ID,
        .defaultIndex = 0,
        .currentIndex = 0,
        .tab = { .max = ARRAYLEN(hero7LensValues) - 1, .names = hero7LensLabels },
        .callback = NULL,
        .displayLabel = "LENS",
        .labels = hero7LensLabels,
        .values = hero7LensValues,
        .optionCount = (uint8_t)ARRAYLEN(hero7LensValues)
    }
};

const goproCamera_t goproCameraHero7 = {
    .model = GOPRO_CAMERA_MODEL_HERO7,
    .settings = hero7Settings,
    .settingCount = (uint8_t)ARRAYLEN(hero7Settings),
};

#endif
