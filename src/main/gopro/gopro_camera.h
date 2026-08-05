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

#pragma once

#include <stdint.h>

#include "cms/cms_menu_gopro.h"

typedef enum {
    GOPRO_CAMERA_MODEL_UNKNOWN = 0,
    GOPRO_CAMERA_MODEL_SESSION5,
    GOPRO_CAMERA_MODEL_HERO7,
    GOPRO_CAMERA_MODEL_HERO8,
    GOPRO_CAMERA_MODEL_HERO9,
    GOPRO_CAMERA_MODEL_HERO10,
    GOPRO_CAMERA_MODEL_HERO11,
    GOPRO_CAMERA_MODEL_HERO12,
    GOPRO_CAMERA_MODEL_HERO13,
    GOPRO_CAMERA_MODEL_MISSION1,
} goproCameraModel_e;

typedef struct {
    goproCameraModel_e model;
    goproCmsSettingTable_t *settings;
    uint8_t settingCount;
} goproCamera_t;

  
void goproCameraSetModel(goproCameraModel_e model);
const goproCamera_t *goproCameraGetCurrent(void);
goproCameraModel_e goproCameraDetectModelFromStatus(const char *statusText);
const goproCamera_t *goproCameraGetModelData(goproCameraModel_e model);