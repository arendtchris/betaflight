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

static const char * const hero12RecordLabels[] = { "STOP", "START" };
static const uint16_t hero12RecordValues[] = { 0, 1 };
static const char * const hero12ResolutionLabels[] = { "5.3K", "5.3K 8:7", "5.3K 4:3", "4K", "4K 8:7", "4K 4:3", "2.7K", "2.7K 4:3", "1080" };
static const uint16_t hero12ResolutionValues[] = { 100, 26, 27, 1, 28, 18, 4, 6, 9 };
static const char * const hero12FpsLabels[] = { "240.0", "200.0", "120.0", "100.0", "60.0", "50.0", "30.0", "25.0", "24.0" };
static const uint16_t hero12FpsValues[] = { 0, 13, 1, 2, 5, 6, 8, 9, 10 };
static const char * const hero12LensLabels[] = { "Wide", "Superview", "Linear", "Max SuperView", "Linear + Horizon Leveling", "HyperView", "Linear + Horizon Lock" };
static const uint16_t hero12LensValues[] = { 0, 3, 4, 7, 8, 9, 10 };
static const char * const hero12HypersmoothLabels[] = { "Off", "Low", "Boost", "Auto Boost" };
static const uint16_t hero12HypersmoothValues[] = { 0, 1, 3, 4 };
static const char * const hero12BitDepthLabels[] = { "8-Bit", "10-Bit" };
static const uint16_t hero12BitDepthValues[] = { 0, 2 };
static const char * const hero12EasyModeLabels[] = { "Highest Quality", "Standard Quality", "Basic Quality", "Standard Video", "HDR Video" };
static const uint16_t hero12EasyModeValues[] = { 0, 1, 2, 3, 4 };
static const char * const hero12LapseModeLabels[] = { "TimeWarp", "Star Trails", "Light Painting", "Vehicle Lights", "Max TimeWarp", "Max Star Trails", "Max Light Painting", "Max Vehicle Lights", "Time Lapse Video", "Night Lapse Video" };
static const uint16_t hero12LapseModeValues[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

static goproCmsSettingTable_t hero12Settings[] = {
    {
        .settingId = GOPRO_SETTING_RECORD,
        .commandId = GOPRO_SETTING_RECORD_ID,
        .defaultIndex = 0,
        .currentIndex = 0,
        .tab = { .max = ARRAYLEN(hero12RecordValues) - 1, .names = hero12RecordLabels },
        .callback = NULL,
        .displayLabel = "RECORD",
        .labels = hero12RecordLabels,
        .values = hero12RecordValues,
        .optionCount = (uint8_t)ARRAYLEN(hero12RecordValues)
    },
    {
        .settingId = GOPRO_SETTING_RESOLUTION,
        .commandId = GOPRO_SETTING_RESOLUTION_ID,
        .defaultIndex = 2,
        .currentIndex = 2,
        .tab = { .max = ARRAYLEN(hero12ResolutionValues) - 1, .names = hero12ResolutionLabels },
        .callback = NULL,
        .displayLabel = "RESOLUTION",
        .labels = hero12ResolutionLabels,
        .values = hero12ResolutionValues,
        .optionCount = (uint8_t)ARRAYLEN(hero12ResolutionValues)
    },
    {
        .settingId = GOPRO_SETTING_FPS,
        .commandId = GOPRO_SETTING_FPS_ID,
        .defaultIndex = 4,
        .currentIndex = 4,
        .tab = { .max = ARRAYLEN(hero12FpsValues) - 1, .names = hero12FpsLabels },
        .callback = NULL,
        .displayLabel = "FPS",
        .labels = hero12FpsLabels,
        .values = hero12FpsValues,
        .optionCount = (uint8_t)ARRAYLEN(hero12FpsValues)
    },
    {
        .settingId = GOPRO_SETTING_LENS,
        .commandId = GOPRO_SETTING_LENS_ID,
        .defaultIndex = 1,
        .currentIndex = 1,
        .tab = { .max = ARRAYLEN(hero12LensValues) - 1, .names = hero12LensLabels },
        .callback = NULL,
        .displayLabel = "LENS",
        .labels = hero12LensLabels,
        .values = hero12LensValues,
        .optionCount = (uint8_t)ARRAYLEN(hero12LensValues)
    },
    {
        .settingId = GOPRO_SETTING_HYPERSMOOTH,
        .commandId = GOPRO_SETTING_HYPERSMOOTH_ID,
        .defaultIndex = 0,
        .currentIndex = 0,
        .tab = { .max = ARRAYLEN(hero12HypersmoothValues) - 1, .names = hero12HypersmoothLabels },
        .callback = NULL,
        .displayLabel = "HYPERSMOOTH",
        .labels = hero12HypersmoothLabels,
        .values = hero12HypersmoothValues,
        .optionCount = (uint8_t)ARRAYLEN(hero12HypersmoothValues)
    },
    {
        .settingId = GOPRO_SETTING_BIT_DEPTH,
        .commandId = GOPRO_SETTING_BIT_DEPTH_ID,
        .defaultIndex = 0,
        .currentIndex = 0,
        .tab = { .max = ARRAYLEN(hero12BitDepthValues) - 1, .names = hero12BitDepthLabels },
        .callback = NULL,
        .displayLabel = "BIT DEPTH",
        .labels = hero12BitDepthLabels,
        .values = hero12BitDepthValues,
        .optionCount = (uint8_t)ARRAYLEN(hero12BitDepthValues)
    },
    {
        .settingId = GOPRO_SETTING_HDR,
        .commandId = GOPRO_SETTING_HDR_ID,
        .defaultIndex = 0,
        .currentIndex = 0,
        .tab = { .max = ARRAYLEN(hero12EasyModeValues) - 1, .names = hero12EasyModeLabels },
        .callback = NULL,
        .displayLabel = "EASY MODE",
        .labels = hero12EasyModeLabels,
        .values = hero12EasyModeValues,
        .optionCount = (uint8_t)ARRAYLEN(hero12EasyModeValues)
    },
    {
        .settingId = GOPRO_SETTING_TIMELAPSE,
        .commandId = GOPRO_SETTING_TIMELAPSE_ID,
        .defaultIndex = 0,
        .currentIndex = 0,
        .tab = { .max = ARRAYLEN(hero12LapseModeValues) - 1, .names = hero12LapseModeLabels },
        .callback = NULL,
        .displayLabel = "LAPSE MODE",
        .labels = hero12LapseModeLabels,
        .values = hero12LapseModeValues,
        .optionCount = (uint8_t)ARRAYLEN(hero12LapseModeValues)
    }
};

const goproCamera_t goproCameraHero12 = {
    .model = GOPRO_CAMERA_MODEL_HERO12,
    .settings = hero12Settings,
    .settingCount = (uint8_t)ARRAYLEN(hero12Settings),
};

#endif
