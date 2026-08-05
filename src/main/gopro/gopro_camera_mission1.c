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

static const char * const mission1RecordLabels[] = { "STOP", "START" };
static const uint16_t mission1RecordValues[] = { 0, 1 };
static const char * const mission1ResolutionLabels[] = { "4K", "2.7K", "1080" };
static const uint16_t mission1ResolutionValues[] = { 1, 4, 9 };
static const char * const mission1FpsLabels[] = { "60", "50", "30", "24" };
static const uint16_t mission1FpsValues[] = { 5, 6, 8, 10 };
static const char * const mission1LensLabels[] = { "WIDE", "LINEAR" };
static const uint16_t mission1LensValues[] = { 0, 4 };
static const char * const mission1BitDepthLabels[] = { "8-Bit", "10-Bit" };
static const uint16_t mission1BitDepthValues[] = { 0, 2 };

static goproCmsSettingTable_t mission1Settings[] = {
    {
        .settingId = GOPRO_SETTING_RECORD,
        .commandId = GOPRO_SETTING_RECORD_ID,
        .defaultIndex = 0,
        .currentIndex = 0,
        .tab = { .max = ARRAYLEN(mission1RecordValues) - 1, .names = mission1RecordLabels },
        .callback = NULL,
        .displayLabel = "RECORD",
        .labels = mission1RecordLabels,
        .values = mission1RecordValues,
        .optionCount = (uint8_t)ARRAYLEN(mission1RecordValues)
    },
    {
        .settingId = GOPRO_SETTING_RESOLUTION,
        .commandId = GOPRO_SETTING_RESOLUTION_ID,
        .defaultIndex = 0,
        .currentIndex = 0,
        .tab = { .max = ARRAYLEN(mission1ResolutionValues) - 1, .names = mission1ResolutionLabels },
        .callback = NULL,
        .displayLabel = "RESOLUTION",
        .labels = mission1ResolutionLabels,
        .values = mission1ResolutionValues,
        .optionCount = (uint8_t)ARRAYLEN(mission1ResolutionValues)
    },
    {
        .settingId = GOPRO_SETTING_FPS,
        .commandId = GOPRO_SETTING_FPS_ID,
        .defaultIndex = 2,
        .currentIndex = 2,
        .tab = { .max = ARRAYLEN(mission1FpsValues) - 1, .names = mission1FpsLabels },
        .callback = NULL,
        .displayLabel = "FPS",
        .labels = mission1FpsLabels,
        .values = mission1FpsValues,
        .optionCount = (uint8_t)ARRAYLEN(mission1FpsValues)
    },
    {
        .settingId = GOPRO_SETTING_LENS,
        .commandId = GOPRO_SETTING_LENS_ID,
        .defaultIndex = 0,
        .currentIndex = 0,
        .tab = { .max = ARRAYLEN(mission1LensValues) - 1, .names = mission1LensLabels },
        .callback = NULL,
        .displayLabel = "LENS",
        .labels = mission1LensLabels,
        .values = mission1LensValues,
        .optionCount = (uint8_t)ARRAYLEN(mission1LensValues)
    },
    {
        .settingId = GOPRO_SETTING_BIT_DEPTH,
        .commandId = GOPRO_SETTING_BIT_DEPTH_ID,
        .defaultIndex = 0,
        .currentIndex = 0,
        .tab = { .max = ARRAYLEN(mission1BitDepthValues) - 1, .names = mission1BitDepthLabels },
        .callback = NULL,
        .displayLabel = "BIT DEPTH",
        .labels = mission1BitDepthLabels,
        .values = mission1BitDepthValues,
        .optionCount = (uint8_t)ARRAYLEN(mission1BitDepthValues)
    }
};

const goproCamera_t goproCameraMission1 = {
    .model = GOPRO_CAMERA_MODEL_MISSION1,
    .settings = mission1Settings,
    .settingCount = (uint8_t)ARRAYLEN(mission1Settings),
};

#endif
