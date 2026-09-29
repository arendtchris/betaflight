/*
 * This file is part of Cleanflight and Betaflight.
 *
 * Cleanflight and Betaflight are free software. You can redistribute
 * this software and/or modify this software under the terms of the
 * GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
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

#ifdef USE_CMS

#include <stdbool.h>
#include <stdint.h>

#include "cms/cms.h"
#include "cms/cms_types.h"
#include "msp/msp_gopro.h"

#define GOPRO_SETTING_RESOLUTION 2
#define GOPRO_SETTING_FPS 3

static const char * const resolutionLabels[] = { "4K", "2.7K", "1080" };
static const uint8_t resolutionValues[] = { 1, 4, 9 };
static const char * const fpsLabels[] = { "60", "50", "30", "25", "24" };
static const uint8_t fpsValues[] = { 5, 6, 8, 9, 10 };

static uint8_t resolutionIndex;
static uint8_t fpsIndex;
static uint8_t resolutionRevision;
static uint8_t fpsRevision;

static OSD_TAB_t resolutionTab = { &resolutionIndex, ARRAYLEN(resolutionLabels) - 1, resolutionLabels };
static OSD_TAB_t fpsTab = { &fpsIndex, ARRAYLEN(fpsLabels) - 1, fpsLabels };

static void cmsx_menuGoproSyncSetting(uint8_t settingId, const uint8_t *values, uint8_t valueCount, uint8_t *selectedIndex, uint8_t *revision)
{
    const uint8_t newRevision = goproMspGetSettingRevision(settingId);
    if (newRevision == *revision) {
        return;
    }

    *revision = newRevision;

    uint8_t value;
    if (!goproMspGetSetting(settingId, &value)) {
        return;
    }

    for (uint8_t index = 0; index < valueCount; index++) {
        if (values[index] == value) {
            *selectedIndex = index;
            break;
        }
    }
}

static const void *cmsx_menuGoproSetResolution(displayPort_t *display, const void *self)
{
    UNUSED(display);
    UNUSED(self);
    if (resolutionIndex < ARRAYLEN(resolutionValues)) {
        goproMspSendSetSetting(GOPRO_SETTING_RESOLUTION, resolutionValues[resolutionIndex]);
    }
    return NULL;
}

static const void *cmsx_menuGoproSetFps(displayPort_t *display, const void *self)
{
    UNUSED(display);
    UNUSED(self);
    if (fpsIndex < ARRAYLEN(fpsValues)) {
        goproMspSendSetSetting(GOPRO_SETTING_FPS, fpsValues[fpsIndex]);
    }
    return NULL;
}

static const void *cmsx_menuGoproOnEnter(displayPort_t *display)
{
    UNUSED(display);

    resolutionRevision = goproMspGetSettingRevision(GOPRO_SETTING_RESOLUTION);
    fpsRevision = goproMspGetSettingRevision(GOPRO_SETTING_FPS);
    goproMspSendGetSetting(GOPRO_SETTING_RESOLUTION);
    goproMspSendGetSetting(GOPRO_SETTING_FPS);
    return NULL;
}

static const void *cmsx_menuGoproOnDisplayUpdate(displayPort_t *display, const OSD_Entry *selected)
{
    UNUSED(display);
    UNUSED(selected);

    cmsx_menuGoproSyncSetting(GOPRO_SETTING_RESOLUTION, resolutionValues, ARRAYLEN(resolutionValues), &resolutionIndex, &resolutionRevision);
    cmsx_menuGoproSyncSetting(GOPRO_SETTING_FPS, fpsValues, ARRAYLEN(fpsValues), &fpsIndex, &fpsRevision);
    return NULL;
}

static const OSD_Entry menuGoproEntries[] = {
    { "RESOLUTION", OME_TAB, cmsx_menuGoproSetResolution, &resolutionTab },
    { "FPS", OME_TAB, cmsx_menuGoproSetFps, &fpsTab },
    { "BACK", OME_Back, NULL, NULL },
    { NULL, OME_END, NULL, NULL }
};

CMS_Menu cmsx_menuGopro = {
#ifdef CMS_MENU_DEBUG
    .GUARD_text = "MENUGOPRO",
    .GUARD_type = OME_MENU,
#endif
    .onEnter = cmsx_menuGoproOnEnter,
    .onExit = NULL,
    .onDisplayUpdate = cmsx_menuGoproOnDisplayUpdate,
    .entries = menuGoproEntries,
};

#endif