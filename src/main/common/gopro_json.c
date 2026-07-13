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

#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "common/printf.h"

#include "common/gopro_json.h"

static const char *goproJsonSkipSpaces(const char *p)
{
    while (p && *p && isspace((unsigned char)*p)) {
        p++;
    }
    return p;
}

bool goproJsonParseUint16(const char *text, uint16_t *out)
{
    uint32_t value = 0;
    bool hasDigit = false;
    const char *p = goproJsonSkipSpaces(text);

    while (p && *p && isdigit((unsigned char)*p)) {
        hasDigit = true;
        value = value * 10 + (uint32_t)(*p - '0');
        if (value > 0xFFFFU) {
            return false;
        }
        p++;
    }

    if (!hasDigit || !out) {
        return false;
    }

    *out = (uint16_t)value;
    return true;
}

bool goproJsonExtractValue(const char *rangeStart, const char *rangeEnd, const char *jsonKey, char *out, size_t outSize)
{
    char pattern[20];
    const size_t keyLen = strlen(jsonKey);

    if (!rangeStart || !rangeEnd || rangeEnd <= rangeStart || !jsonKey || keyLen == 0 || keyLen > 14 || !out || outSize == 0) {
        return false;
    }

    tfp_sprintf(pattern, "\"%s\"", jsonKey);
    const size_t patternLen = strlen(pattern);

    const char *keyPos = rangeStart;
    while (keyPos && keyPos < rangeEnd) {
        keyPos = strstr(keyPos, pattern);
        if (!keyPos || keyPos >= rangeEnd) {
            return false;
        }

        const char *colon = strchr(keyPos + patternLen, ':');
        if (!colon || colon >= rangeEnd) {
            return false;
        }

        const char *value = goproJsonSkipSpaces(colon + 1);
        if (!value || value >= rangeEnd) {
            return false;
        }

        if (*value == '"') {
            value++;
            const char *valueEnd = value;
            while (valueEnd < rangeEnd && *valueEnd != '"') {
                valueEnd++;
            }

            size_t valueLen = (size_t)(valueEnd - value);
            if (valueLen > 0 && valueLen < outSize) {
                memcpy(out, value, valueLen);
                out[valueLen] = '\0';
                return true;
            }
        } else {
            const char *valueEnd = value;
            while (valueEnd < rangeEnd && *valueEnd != ',' && *valueEnd != '}' && !isspace((unsigned char)*valueEnd)) {
                valueEnd++;
            }

            size_t valueLen = (size_t)(valueEnd - value);
            if (valueLen > 0 && valueLen < outSize) {
                memcpy(out, value, valueLen);
                out[valueLen] = '\0';
                return true;
            }
        }

        keyPos += patternLen;
    }

    return false;
}

bool goproJsonExtractObjectRange(const char *jsonText, const char *objectName, const char **objectStart, const char **objectEnd)
{
    char pattern[20];
    const size_t objectNameLen = strlen(objectName);

    if (!jsonText || !objectName || !objectStart || !objectEnd || objectNameLen == 0 || objectNameLen > 14) {
        return false;
    }

    tfp_sprintf(pattern, "\"%s\"", objectName);

    const char *keyPos = strstr(jsonText, pattern);
    if (!keyPos) {
        return false;
    }

    const char *cursor = strchr(keyPos + strlen(pattern), ':');
    if (!cursor) {
        return false;
    }

    cursor = goproJsonSkipSpaces(cursor + 1);
    if (!cursor || *cursor != '{') {
        return false;
    }

    const char *start = cursor;
    int depth = 0;
    for (; *cursor; cursor++) {
        if (*cursor == '{') {
            depth++;
        } else if (*cursor == '}') {
            depth--;
            if (depth == 0) {
                *objectStart = start;
                *objectEnd = cursor;
                return true;
            }
        }
    }

    return false;
}

bool goproJsonExtractUint16(const char *rangeStart, const char *rangeEnd, const char *jsonKey, uint16_t *out)
{
    char value[16];

    if (!out) {
        return false;
    }

    if (!goproJsonExtractValue(rangeStart, rangeEnd, jsonKey, value, sizeof(value))) {
        return false;
    }

    return goproJsonParseUint16(value, out);
}

bool goproJsonGetSettingOption(const char *jsonText, uint16_t settingId, uint16_t *out)
{
    char settingKey[8];
    const char *settingsStart;
    const char *settingsEnd;

    if (!jsonText || !out) {
        return false;
    }

    if (!goproJsonExtractObjectRange(jsonText, "settings", &settingsStart, &settingsEnd)) {
        return false;
    }

    tfp_sprintf(settingKey, "%u", settingId);
    return goproJsonExtractUint16(settingsStart, settingsEnd, settingKey, out);
}
