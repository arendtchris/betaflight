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
#include <strings.h>

#include "common/gopro_json.h"
#include "gopro/gopro_camera.h"

extern const goproCamera_t goproCameraHero8;
extern const goproCamera_t goproCameraHero9;
extern const goproCamera_t goproCameraHero10;
extern const goproCamera_t goproCameraHero11;
extern const goproCamera_t goproCameraHero12;
extern const goproCamera_t goproCameraHero13;
extern const goproCamera_t goproCameraMission1;
extern const goproCamera_t goproCameraHero7;
extern const goproCamera_t goproCameraSession5;

static const goproCamera_t *currentCamera = NULL;

const goproCamera_t *goproCameraGetModelData(goproCameraModel_e model)
{
    switch (model) {
    case GOPRO_CAMERA_MODEL_HERO8:
        return &goproCameraHero8;
    case GOPRO_CAMERA_MODEL_HERO9:
        return &goproCameraHero9;
    case GOPRO_CAMERA_MODEL_HERO10:
        return &goproCameraHero10;
    case GOPRO_CAMERA_MODEL_HERO11:
        return &goproCameraHero11;
    case GOPRO_CAMERA_MODEL_HERO12:
        return &goproCameraHero12;
    case GOPRO_CAMERA_MODEL_HERO13:
        return &goproCameraHero13;
    case GOPRO_CAMERA_MODEL_MISSION1:
        return &goproCameraMission1;
    case GOPRO_CAMERA_MODEL_HERO7:
        return &goproCameraHero7;
    case GOPRO_CAMERA_MODEL_SESSION5:
        return &goproCameraSession5;
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

goproCameraModel_e goproCameraDetectModelFromStatus(const char *goproModel)
{
    
    if (!goproModel || !goproModel[0]) {
        return GOPRO_CAMERA_MODEL_UNKNOWN;
    }

    if (  strcasecmp(goproModel, "HERO8 Black") == 0) {
        return GOPRO_CAMERA_MODEL_HERO8;
    }

    if (strcasecmp(goproModel, "HERO7 Black") == 0 ) {
        return GOPRO_CAMERA_MODEL_HERO7;
    }

    if (strcasecmp(goproModel, "HERO9 Black") == 0) {
        return GOPRO_CAMERA_MODEL_HERO9;
    }

    if (strcasecmp(goproModel, "HERO10 Black") == 0) { 
        return GOPRO_CAMERA_MODEL_HERO10;
    }
    if (strcasecmp(goproModel, "HERO11 Black Mini") == 0) {
        return GOPRO_CAMERA_MODEL_HERO11;
    }
    if (strcasecmp(goproModel, "HERO11 Black") == 0 ) {
        return GOPRO_CAMERA_MODEL_HERO11;
    }
   

    if (strcasecmp(goproModel, "HERO12 Black") == 0) {
        return GOPRO_CAMERA_MODEL_HERO12;
    }

    if (strcasecmp(goproModel, "HERO13 Black") == 0) {
        return GOPRO_CAMERA_MODEL_HERO13;
    }

    if (strcasecmp(goproModel, "MISSION 1") == 0) {
        return GOPRO_CAMERA_MODEL_MISSION1;
    }

    if (strcasecmp(goproModel, "HERO5 Session") == 0) {
        return GOPRO_CAMERA_MODEL_SESSION5;
    }

    return GOPRO_CAMERA_MODEL_UNKNOWN;
}

#endif
