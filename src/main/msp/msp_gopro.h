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

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "msp/msp.h"

#define GOPRO_MSP_MAX_SETTING_CAPABILITIES 32

bool goproMspSendGetSetting(uint8_t settingId);
bool goproMspSendSetSetting(uint8_t settingId, uint8_t value);
bool goproMspSendGetSettingCapabilities(uint8_t settingId);
bool goproMspGetSetting(uint8_t settingId, uint8_t *value);
uint8_t goproMspGetSettingRevision(uint8_t settingId);
const uint8_t *goproMspGetSettingCapabilities(uint8_t settingId, uint8_t *count);
bool goproMspTakeSettingCapabilitiesUpdate(uint8_t settingId);
void goproMspProcessReply(mspPacket_t *reply);