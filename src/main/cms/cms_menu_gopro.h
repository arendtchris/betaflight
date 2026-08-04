/*
 * This file is part of Cleanflight and Betaflight.
 *
 * Cleanflight and Betaflight are free software. You can redistribute
 * this software and/or modify this software under the terms of the
 * GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * Cleanflight and Betaflight are distributed in the hope that they
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software.
 *
 * If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <stdint.h>

#include "cms/cms_types.h"

typedef enum {
    GOPRO_SETTING_RECORD,
    GOPRO_SETTING_RESOLUTION,
    GOPRO_SETTING_FPS,
    GOPRO_SETTING_LENS,
    GOPRO_SETTING_HYPERSMOOTH,
    GOPRO_SETTING_COUNT
} goproCmsSettingId_e;

typedef struct {
    goproCmsSettingId_e settingId;
    uint16_t commandId;
    uint8_t defaultIndex;
    uint8_t currentIndex;
    OSD_TAB_t tab;

    CMSEntryFuncPtr callback;
    const char *displayLabel;
    const char * const *labels;
    const uint16_t *values;
    uint8_t optionCount;
} goproCmsSettingTable_t;

const void *cmsx_menuGoproSendSetting(goproCmsSettingId_e settingId);

extern CMS_Menu cmsx_menuGopro;