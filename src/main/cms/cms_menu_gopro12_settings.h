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

#define GOPRO_SETTING_RECORD_ID 8
#define GOPRO_SETTING_RESOLUTION_ID 2
#define GOPRO_SETTING_FPS_ID 3
#define GOPRO_SETTING_LENS_ID 121
#define GOPRO_SETTING_HYPERSMOOTH_ID 135

goproCmsSettingTable_t *cmsMenuGopro12GetSettings(void);
uint8_t cmsMenuGopro12GetSettingsCount(void);
goproCmsSettingTable_t *cmsMenuGopro12GetSetting(goproCmsSettingId_e settingId);
