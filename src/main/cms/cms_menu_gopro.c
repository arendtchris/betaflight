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

#include "platform.h"

#if defined(USE_OSD) && defined(USE_CMS)

#include <stdbool.h>
#include <string.h>

#include "common/printf.h"

#include "cms/cms.h"
#include "cms/cms_types.h"
#include "cms/cms_menu_gopro.h"
#include "osd/osd_gopro_status.h"

static const void *cmsx_menuGoproSendCommand(displayPort_t *pDisp, const void *ptr)
{
    UNUSED(pDisp);

    const char *commandParam = (const char *)ptr;
    if (commandParam && commandParam[0]) {
        
        osdGoproStatusSendCommand(commandParam);
    }

    return NULL;
}

static const OSD_Entry cmsx_menuGoproEntries[] =
{
    {"--- GOPRO FPS ---", OME_Label, NULL, NULL},
    {"240.0", OME_Funcall, cmsx_menuGoproSendCommand, (void *)"option=0&setting=3"},
    {"120.0", OME_Funcall, cmsx_menuGoproSendCommand, (void *)"option=1&setting=3"},
    {"100.0", OME_Funcall, cmsx_menuGoproSendCommand, (void *)"option=2&setting=3"},
    {"90.0",  OME_Funcall, cmsx_menuGoproSendCommand, (void *)"option=3&setting=3"},
    {"60.0",  OME_Funcall, cmsx_menuGoproSendCommand, (void *)"option=5&setting=3"},
    {"50.0",  OME_Funcall, cmsx_menuGoproSendCommand, (void *)"option=6&setting=3"},
    {"30.0",  OME_Funcall, cmsx_menuGoproSendCommand, (void *)"option=8&setting=3"},
    {"25.0",  OME_Funcall, cmsx_menuGoproSendCommand, (void *)"option=9&setting=3"},
    {"24.0",  OME_Funcall, cmsx_menuGoproSendCommand, (void *)"option=10&setting=3"},
    {"200.0", OME_Funcall, cmsx_menuGoproSendCommand, (void *)"option=13&setting=3"},
    {"400.0", OME_Funcall, cmsx_menuGoproSendCommand, (void *)"option=15&setting=3"},
    {"360.0", OME_Funcall, cmsx_menuGoproSendCommand, (void *)"option=16&setting=3"},
    {"300.0", OME_Funcall, cmsx_menuGoproSendCommand, (void *)"option=17&setting=3"},
    {"480.0", OME_Funcall, cmsx_menuGoproSendCommand, (void *)"option=18&setting=3"},
    {"960.0", OME_Funcall, cmsx_menuGoproSendCommand, (void *)"option=19&setting=3"},
    {"800.0", OME_Funcall, cmsx_menuGoproSendCommand, (void *)"option=20&setting=3"},
    {"-- HYPERSMOOTH --", OME_Label, NULL, NULL},
    {"OFF", OME_Funcall, cmsx_menuGoproSendCommand, (void *)"option=0&setting=135"},
    {"LOW", OME_Funcall, cmsx_menuGoproSendCommand, (void *)"option=1&setting=135"},
    {"AUTO BOOST", OME_Funcall, cmsx_menuGoproSendCommand, (void *)"option=4&setting=135"},
    {"BACK", OME_Back, NULL, NULL},
    {NULL, OME_END, NULL, NULL}
};

CMS_Menu cmsx_menuGopro = {
#ifdef CMS_MENU_DEBUG
    .GUARD_text = "MENUGOPRO",
    .GUARD_type = OME_MENU,
#endif
    .onEnter = NULL,
    .onExit = NULL,
    .onDisplayUpdate = NULL,
    .entries = cmsx_menuGoproEntries
};

#endif