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

static const char * const hero13RecordLabels[] = { "STOP", "START" };
static const uint16_t hero13RecordValues[] = { 0, 1 };
static const char * const hero13ResolutionLabels[] = { "5.3K", "5.3K 8:7", "5.3K 4:3", "4K", "4K 8:7", "4K 4:3", "2.7K", "2.7K 4:3", "1080" };
static const uint16_t hero13ResolutionValues[] = { 100, 26, 27, 1, 28, 18, 4, 6, 9 };
static const char * const hero13FpsLabels[] = { "240.0", "200.0", "120.0", "100.0", "60.0", "50.0", "30.0", "25.0", "24.0" };
static const uint16_t hero13FpsValues[] = { 0, 13, 1, 2, 5, 6, 8, 9, 10 };
static const char * const hero13LensLabels[] = { "Wide", "Superview", "Linear", "Max SuperView", "Linear + Horizon Leveling", "HyperView", "Linear + Horizon Lock" };
static const uint16_t hero13LensValues[] = { 0, 3, 4, 7, 8, 9, 10 };
static const char * const hero13HypersmoothLabels[] = { "Off", "Low", "Boost", "Auto Boost" };
static const uint16_t hero13HypersmoothValues[] = { 0, 1, 3, 4 };
static const char * const hero13BitDepthLabels[] = { "8-Bit", "10-Bit" };
static const uint16_t hero13BitDepthValues[] = { 0, 2 };
static const char * const hero13EasyModeLabels[] = { "Highest Quality", "Standard Quality", "Basic Quality", "Standard Video", "HDR Video" };
static const uint16_t hero13EasyModeValues[] = { 0, 1, 2, 3, 4 };
static const char * const hero13LapseModeLabels[] = { "TimeWarp", "Star Trails", "Light Painting", "Vehicle Lights", "Max TimeWarp", "Max Star Trails", "Max Light Painting", "Max Vehicle Lights", "Time Lapse Video", "Night Lapse Video" };
static const uint16_t hero13LapseModeValues[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

static goproCmsSettingTable_t hero13Settings[] = {
    {
        .settingId = GOPRO_SETTING_RECORD,
        .commandId = GOPRO_SETTING_RECORD_ID,
        .defaultIndex = 0,
        .currentIndex = 0,
        .tab = { .max = ARRAYLEN(hero13RecordValues) - 1, .names = hero13RecordLabels },
        .callback = NULL,
        .displayLabel = "RECORD",
        .labels = hero13RecordLabels,
        .values = hero13RecordValues,
        .optionCount = (uint8_t)ARRAYLEN(hero13RecordValues)
    },
    {
        .settingId = GOPRO_SETTING_RESOLUTION,
        .commandId = GOPRO_SETTING_RESOLUTION_ID,
        .defaultIndex = 2,
        .currentIndex = 2,
        .tab = { .max = ARRAYLEN(hero13ResolutionValues) - 1, .names = hero13ResolutionLabels },
        .callback = NULL,
        .displayLabel = "RESOLUTION",
        .labels = hero13ResolutionLabels,
        .values = hero13ResolutionValues,
        .optionCount = (uint8_t)ARRAYLEN(hero13ResolutionValues)
    },
    {
        .settingId = GOPRO_SETTING_FPS,
        .commandId = GOPRO_SETTING_FPS_ID,
        .defaultIndex = 4,
        .currentIndex = 4,
        .tab = { .max = ARRAYLEN(hero13FpsValues) - 1, .names = hero13FpsLabels },
        .callback = NULL,
        .displayLabel = "FPS",
        .labels = hero13FpsLabels,
        .values = hero13FpsValues,
        .optionCount = (uint8_t)ARRAYLEN(hero13FpsValues)
    },
    {
        .settingId = GOPRO_SETTING_LENS,
        .commandId = GOPRO_SETTING_LENS_ID,
        .defaultIndex = 1,
        .currentIndex = 1,
        .tab = { .max = ARRAYLEN(hero13LensValues) - 1, .names = hero13LensLabels },
        .callback = NULL,
        .displayLabel = "LENS",
        .labels = hero13LensLabels,
        .values = hero13LensValues,
        .optionCount = (uint8_t)ARRAYLEN(hero13LensValues)
    },
    {
        .settingId = GOPRO_SETTING_HYPERSMOOTH,
        .commandId = GOPRO_SETTING_HYPERSMOOTH_ID,
        .defaultIndex = 0,
        .currentIndex = 0,
        .tab = { .max = ARRAYLEN(hero13HypersmoothValues) - 1, .names = hero13HypersmoothLabels },
        .callback = NULL,
        .displayLabel = "HYPERSMOOTH",
        .labels = hero13HypersmoothLabels,
        .values = hero13HypersmoothValues,
        .optionCount = (uint8_t)ARRAYLEN(hero13HypersmoothValues)
    },
    {
        .settingId = GOPRO_SETTING_BIT_DEPTH,
        .commandId = GOPRO_SETTING_BIT_DEPTH_ID,
        .defaultIndex = 0,
        .currentIndex = 0,
        .tab = { .max = ARRAYLEN(hero13BitDepthValues) - 1, .names = hero13BitDepthLabels },
        .callback = NULL,
        .displayLabel = "BIT DEPTH",
        .labels = hero13BitDepthLabels,
        .values = hero13BitDepthValues,
        .optionCount = (uint8_t)ARRAYLEN(hero13BitDepthValues)
    },
    {
        .settingId = GOPRO_SETTING_HDR,
        .commandId = GOPRO_SETTING_HDR_ID,
        .defaultIndex = 0,
        .currentIndex = 0,
        .tab = { .max = ARRAYLEN(hero13EasyModeValues) - 1, .names = hero13EasyModeLabels },
        .callback = NULL,
        .displayLabel = "EASY MODE",
        .labels = hero13EasyModeLabels,
        .values = hero13EasyModeValues,
        .optionCount = (uint8_t)ARRAYLEN(hero13EasyModeValues)
    },
    {
        .settingId = GOPRO_SETTING_TIMELAPSE,
        .commandId = GOPRO_SETTING_TIMELAPSE_ID,
        .defaultIndex = 0,
        .currentIndex = 0,
        .tab = { .max = ARRAYLEN(hero13LapseModeValues) - 1, .names = hero13LapseModeLabels },
        .callback = NULL,
        .displayLabel = "LAPSE MODE",
        .labels = hero13LapseModeLabels,
        .values = hero13LapseModeValues,
        .optionCount = (uint8_t)ARRAYLEN(hero13LapseModeValues)
    }
};

const goproCamera_t goproCameraHero13 = {
    .model = GOPRO_CAMERA_MODEL_HERO13,
    .settings = hero13Settings,
    .settingCount = (uint8_t)ARRAYLEN(hero13Settings),
};

#endif
