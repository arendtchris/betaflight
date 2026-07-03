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

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "platform.h"

#if defined(USE_OSD)

#include "drivers/serial.h"
#include "io/serial.h"
#include "common/time.h"

#define GOPRO_INPUT_BUFFER_SIZE 64
#define GOPRO_DISPLAY_BUFFER_SIZE 32

static serialPort_t *goproSerialPort = NULL;
static char goproInputBuffer[GOPRO_INPUT_BUFFER_SIZE];
static uint8_t goproInputPos = 0;
static char goproDisplayBuffer[GOPRO_DISPLAY_BUFFER_SIZE];

bool osdGoproStatusInit(void)
{
    const serialPortConfig_t *portConfig = findSerialPortConfig(FUNCTION_OSD_GOPRO_STATUS);
    if (!portConfig) {
        return false;
    }

    uint8_t baudrateIndex = portConfig->gps_baudrateIndex;
    if (baudrateIndex >= BAUD_COUNT) {
        baudrateIndex = BAUD_57600;
    }
    const uint32_t baudrate = baudRates[baudrateIndex];

    goproSerialPort = openSerialPort(
        portConfig->identifier,
        FUNCTION_OSD_GOPRO_STATUS,
        NULL,
        NULL,
        baudrate,
        MODE_RX,
        SERIAL_NOT_INVERTED
    );

    if (!goproSerialPort) {
        return false;
    }

    memset(goproInputBuffer, 0, sizeof(goproInputBuffer));
    memset(goproDisplayBuffer, 0, sizeof(goproDisplayBuffer));
    goproInputPos = 0;

    return true;
}

void osdGoproStatusUpdate(timeUs_t currentTimeUs)
{
    UNUSED(currentTimeUs);

    if (!goproSerialPort) {
        return;
    }

    while (serialRxBytesWaiting(goproSerialPort)) {
        const uint8_t c = serialRead(goproSerialPort);

        if (c == '\n') {
            if (goproInputPos > 0) {
                const uint8_t copyLen = (goproInputPos < (GOPRO_DISPLAY_BUFFER_SIZE - 1)) ? goproInputPos : (GOPRO_DISPLAY_BUFFER_SIZE - 1);
                memcpy(goproDisplayBuffer, goproInputBuffer, copyLen);
                goproDisplayBuffer[copyLen] = '\0';
            } else {
                goproDisplayBuffer[0] = '\0';
            }
            goproInputPos = 0;
        } else if (c == '\r') {
            // Ignore
        } else if (c != 0x00 && goproInputPos < (GOPRO_INPUT_BUFFER_SIZE - 1)) {
            goproInputBuffer[goproInputPos++] = c;
        }
    }
}

const char *osdGoproStatusGet(void)
{
    return goproDisplayBuffer;
}

#endif
