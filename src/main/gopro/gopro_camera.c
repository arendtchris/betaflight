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

#include <string.h>

#include "gopro/gopro_camera.h"

extern const goproCamera_t goproCameraHero11;
extern const goproCamera_t goproCameraHero12;

static const goproCamera_t *currentCamera = NULL;

const goproCamera_t *goproCameraGetModelData(goproCameraModel_e model)
{
    switch (model) {
    case GOPRO_CAMERA_MODEL_HERO11:
        return &goproCameraHero11;
    case GOPRO_CAMERA_MODEL_HERO12:
        return &goproCameraHero12;
    default:
        return &goproCameraHero11;
    }
}

void goproCameraSetModel(goproCameraModel_e model)
{
    currentCamera = goproCameraGetModelData(model);
}

const goproCamera_t *goproCameraGetCurrent(void)
{
    if (!currentCamera) {
        currentCamera = goproCameraGetModelData(GOPRO_CAMERA_MODEL_HERO11);
    }

    return currentCamera;
}

goproCameraModel_e goproCameraDetectModelFromStatus(const char *statusText)
{
    if (!statusText || !statusText[0]) {
        return GOPRO_CAMERA_MODEL_UNKNOWN;
    }

    if (strstr(statusText, "HERO11") || strstr(statusText, "HER11")) {
        return GOPRO_CAMERA_MODEL_HERO11;
    }

    if (strstr(statusText, "HERO12") || strstr(statusText, "HER12")) {
        return GOPRO_CAMERA_MODEL_HERO12;
    }

    return GOPRO_CAMERA_MODEL_UNKNOWN;
}

#endif
