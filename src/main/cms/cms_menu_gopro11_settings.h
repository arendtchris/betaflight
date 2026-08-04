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

goproCmsSettingTable_t *cmsMenuGoproGetSettings(void);
uint8_t cmsMenuGoproGetSettingsCount(void);
goproCmsSettingTable_t *cmsMenuGoproGetSetting(goproCmsSettingId_e settingId);
const void *cmsMenuGoproRecordCallback(displayPort_t *pDisp, const void *self);
const void *cmsMenuGoproResolutionCallback(displayPort_t *pDisp, const void *self);
const void *cmsMenuGoproFpsCallback(displayPort_t *pDisp, const void *self);
const void *cmsMenuGoproLensCallback(displayPort_t *pDisp, const void *self);
const void *cmsMenuGoproHypersmoothCallback(displayPort_t *pDisp, const void *self);
