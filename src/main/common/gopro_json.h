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

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

bool goproJsonParseUint16(const char *text, uint16_t *out);
bool goproJsonExtractValue(const char *rangeStart, const char *rangeEnd, const char *jsonKey, char *out, size_t outSize);
bool goproJsonExtractObjectRange(const char *jsonText, const char *objectName, const char **objectStart, const char **objectEnd);
bool goproJsonExtractUint16(const char *rangeStart, const char *rangeEnd, const char *jsonKey, uint16_t *out);
bool goproJsonGetSettingOption(const char *jsonText, uint16_t settingId, uint16_t *out);
