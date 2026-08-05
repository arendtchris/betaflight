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

static const char * const hero11RecordLabels[] = { "STOP", "START" };
static const uint16_t hero11RecordValues[] = { 0, 1 };
static const char * const hero11ResolutionLabels[] = { "5.3K", "5.3K 8:7", "5.3K 4:3", "4K", "4K 8:7", "4K 4:3", "2.7K", "2.7K 4:3", "1080" };
static const uint16_t hero11ResolutionValues[] = { 100, 26, 27, 1, 28, 18, 4, 6, 9 };
static const char * const hero11FpsLabels[] = { "240", "200", "120", "100", "60 ", "50 ", "30 ", "25 ", "24 " };
static const uint16_t hero11FpsValues[] = { 0, 13, 1, 2, 5, 6, 8, 9, 10 };
static const char * const hero11LensLabels[] = { "WIDE", "SUPERVIEW", "LINEAR", "MAX SV", "LIN HL", "HYPERVIEW", "LIN LOCK" };
static const uint16_t hero11LensValues[] = { 0, 3, 4, 7, 8, 9, 10 };
static const char * const hero11HypersmoothLabels[] = { "OFF", "LOW", "BOOST", "AUTO" };
static const uint16_t hero11HypersmoothValues[] = { 0, 1, 3, 4 };

static goproCmsSettingTable_t hero11Settings[] = {
    {
        .settingId = GOPRO_SETTING_RECORD,
        .commandId = GOPRO_SETTING_RECORD_ID,
        .defaultIndex = 0,
        .currentIndex = 0,
        .tab = { .max = ARRAYLEN(hero11RecordValues) - 1, .names = hero11RecordLabels },
        .callback = NULL,
        .displayLabel = "RECORD",
        .labels = hero11RecordLabels,
        .values = hero11RecordValues,
        .optionCount = (uint8_t)ARRAYLEN(hero11RecordValues)
    },
    {
        .settingId = GOPRO_SETTING_RESOLUTION,
        .commandId = GOPRO_SETTING_RESOLUTION_ID,
        .defaultIndex = 2,
        .currentIndex = 2,
        .tab = { .max = ARRAYLEN(hero11ResolutionValues) - 1, .names = hero11ResolutionLabels },
        .callback = NULL,
        .displayLabel = "RESOLUTION",
        .labels = hero11ResolutionLabels,
        .values = hero11ResolutionValues,
        .optionCount = (uint8_t)ARRAYLEN(hero11ResolutionValues)
    },
    {
        .settingId = GOPRO_SETTING_FPS,
        .commandId = GOPRO_SETTING_FPS_ID,
        .defaultIndex = 4,
        .currentIndex = 4,
        .tab = { .max = ARRAYLEN(hero11FpsValues) - 1, .names = hero11FpsLabels },
        .callback = NULL,
        .displayLabel = "FPS",
        .labels = hero11FpsLabels,
        .values = hero11FpsValues,
        .optionCount = (uint8_t)ARRAYLEN(hero11FpsValues)
    },
    {
        .settingId = GOPRO_SETTING_LENS,
        .commandId = GOPRO_SETTING_LENS_ID,
        .defaultIndex = 1,
        .currentIndex = 1,
        .tab = { .max = ARRAYLEN(hero11LensValues) - 1, .names = hero11LensLabels },
        .callback = NULL,
        .displayLabel = "LENS",
        .labels = hero11LensLabels,
        .values = hero11LensValues,
        .optionCount = (uint8_t)ARRAYLEN(hero11LensValues)
    },
    {
        .settingId = GOPRO_SETTING_HYPERSMOOTH,
        .commandId = GOPRO_SETTING_HYPERSMOOTH_ID,
        .defaultIndex = 0,
        .currentIndex = 0,
        .tab = { .max = ARRAYLEN(hero11HypersmoothValues) - 1, .names = hero11HypersmoothLabels },
        .callback = NULL,
        .displayLabel = "HYPERSMOOTH",
        .labels = hero11HypersmoothLabels,
        .values = hero11HypersmoothValues,
        .optionCount = (uint8_t)ARRAYLEN(hero11HypersmoothValues)
    }
};

const goproCamera_t goproCameraHero11 = {
    .model = GOPRO_CAMERA_MODEL_HERO11,
    .settings = hero11Settings,
    .settingCount = (uint8_t)ARRAYLEN(hero11Settings),
};

#endif
