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

static const char * const hero10RecordLabels[] = { "STOP", "START" };
static const uint16_t hero10RecordValues[] = { 0, 1 };
static const char * const hero10ResolutionLabels[] = { "5.3K", "4K", "2.7K", "1080" };
static const uint16_t hero10ResolutionValues[] = { 100, 1, 4, 9 };
static const char * const hero10FpsLabels[] = { "60", "50", "30", "24" };
static const uint16_t hero10FpsValues[] = { 5, 6, 8, 10 };
static const char * const hero10LensLabels[] = { "WIDE", "LINEAR", "SUPERVIEW" };
static const uint16_t hero10LensValues[] = { 0, 4, 3 };

static goproCmsSettingTable_t hero10Settings[] = {
    {
        .settingId = GOPRO_SETTING_RECORD,
        .commandId = GOPRO_SETTING_RECORD_ID,
        .defaultIndex = 0,
        .currentIndex = 0,
        .tab = { .max = ARRAYLEN(hero10RecordValues) - 1, .names = hero10RecordLabels },
        .callback = NULL,
        .displayLabel = "RECORD",
        .labels = hero10RecordLabels,
        .values = hero10RecordValues,
        .optionCount = (uint8_t)ARRAYLEN(hero10RecordValues)
    },
    {
        .settingId = GOPRO_SETTING_RESOLUTION,
        .commandId = GOPRO_SETTING_RESOLUTION_ID,
        .defaultIndex = 1,
        .currentIndex = 1,
        .tab = { .max = ARRAYLEN(hero10ResolutionValues) - 1, .names = hero10ResolutionLabels },
        .callback = NULL,
        .displayLabel = "RESOLUTION",
        .labels = hero10ResolutionLabels,
        .values = hero10ResolutionValues,
        .optionCount = (uint8_t)ARRAYLEN(hero10ResolutionValues)
    },
    {
        .settingId = GOPRO_SETTING_FPS,
        .commandId = GOPRO_SETTING_FPS_ID,
        .defaultIndex = 2,
        .currentIndex = 2,
        .tab = { .max = ARRAYLEN(hero10FpsValues) - 1, .names = hero10FpsLabels },
        .callback = NULL,
        .displayLabel = "FPS",
        .labels = hero10FpsLabels,
        .values = hero10FpsValues,
        .optionCount = (uint8_t)ARRAYLEN(hero10FpsValues)
    },
    {
        .settingId = GOPRO_SETTING_LENS,
        .commandId = GOPRO_SETTING_LENS_ID,
        .defaultIndex = 0,
        .currentIndex = 0,
        .tab = { .max = ARRAYLEN(hero10LensValues) - 1, .names = hero10LensLabels },
        .callback = NULL,
        .displayLabel = "LENS",
        .labels = hero10LensLabels,
        .values = hero10LensValues,
        .optionCount = (uint8_t)ARRAYLEN(hero10LensValues)
    }
};

const goproCamera_t goproCameraHero10 = {
    .model = GOPRO_CAMERA_MODEL_HERO10,
    .settings = hero10Settings,
    .settingCount = (uint8_t)ARRAYLEN(hero10Settings),
};

#endif
