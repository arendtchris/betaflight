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

static const char * const session5RecordLabels[] = { "STOP", "START" };
static const uint16_t session5RecordValues[] = { 0, 1 };
static const char * const session5ResolutionLabels[] = { "4K", "2.7K", "1080" };
static const uint16_t session5ResolutionValues[] = { 1, 4, 9 };
static const char * const session5FpsLabels[] = { "60", "50", "30", "24" };
static const uint16_t session5FpsValues[] = { 5, 6, 8, 10 };
static const char * const session5LensLabels[] = { "WIDE", "LINEAR" };
static const uint16_t session5LensValues[] = { 0, 4 };

static goproCmsSettingTable_t session5Settings[] = {
    {
        .settingId = GOPRO_SETTING_RECORD,
        .commandId = GOPRO_SETTING_RECORD_ID,
        .defaultIndex = 0,
        .currentIndex = 0,
        .tab = { .max = ARRAYLEN(session5RecordValues) - 1, .names = session5RecordLabels },
        .callback = NULL,
        .displayLabel = "RECORD",
        .labels = session5RecordLabels,
        .values = session5RecordValues,
        .optionCount = (uint8_t)ARRAYLEN(session5RecordValues)
    },
    {
        .settingId = GOPRO_SETTING_RESOLUTION,
        .commandId = GOPRO_SETTING_RESOLUTION_ID,
        .defaultIndex = 1,
        .currentIndex = 1,
        .tab = { .max = ARRAYLEN(session5ResolutionValues) - 1, .names = session5ResolutionLabels },
        .callback = NULL,
        .displayLabel = "RESOLUTION",
        .labels = session5ResolutionLabels,
        .values = session5ResolutionValues,
        .optionCount = (uint8_t)ARRAYLEN(session5ResolutionValues)
    },
    {
        .settingId = GOPRO_SETTING_FPS,
        .commandId = GOPRO_SETTING_FPS_ID,
        .defaultIndex = 2,
        .currentIndex = 2,
        .tab = { .max = ARRAYLEN(session5FpsValues) - 1, .names = session5FpsLabels },
        .callback = NULL,
        .displayLabel = "FPS",
        .labels = session5FpsLabels,
        .values = session5FpsValues,
        .optionCount = (uint8_t)ARRAYLEN(session5FpsValues)
    },
    {
        .settingId = GOPRO_SETTING_LENS,
        .commandId = GOPRO_SETTING_LENS_ID,
        .defaultIndex = 0,
        .currentIndex = 0,
        .tab = { .max = ARRAYLEN(session5LensValues) - 1, .names = session5LensLabels },
        .callback = NULL,
        .displayLabel = "LENS",
        .labels = session5LensLabels,
        .values = session5LensValues,
        .optionCount = (uint8_t)ARRAYLEN(session5LensValues)
    }
};

const goproCamera_t goproCameraSession5 = {
    .model = GOPRO_CAMERA_MODEL_SESSION5,
    .settings = session5Settings,
    .settingCount = (uint8_t)ARRAYLEN(session5Settings),
};

#endif
