/**
* =============================================================================
* Source Python Extensions
* Copyright (C) 2011 Deniz "your-name-here" Sezen.  All rights reserved.
* =============================================================================
*
* This program is free software; you can redistribute it and/or modify it under
* the terms of the GNU General Public License, version 3.0, as published by the
* Free Software Foundation.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
* details.
*
* You should have received a copy of the GNU General Public License along with
* this program.  If not, see <http://www.gnu.org/licenses/>.
*
* As a special exception, I (Deniz Sezen) give you permission to link the
* code of this program (as well as its derivative works) to "Half-Life 2," the
* "Source Engine," and any Game MODs that run on software
* by the Valve Corporation.  You must obey the GNU General Public License in
* all respects for all other code used.  Additionally, I (Deniz Sezen) grants
* this exception to all derivative works.
*/

//=================================================================================
// Includes
//=================================================================================
// SDK
#include "dbg.h"

// SPE
#include "spe_exceptions.h"

int exceptionHandler(EXCEPTION_POINTERS* info, unsigned long code)
{
    DevMsg(0, "[SPE] ==============================\n");
    DevMsg(0, "[SPE] Caught a critical exception!\n");
    DevMsg(0, "[SPE] Exception code   : %u\n", code);
    DevMsg(0, "[SPE] Exception address: %p\n", info->ExceptionRecord->ExceptionAddress);
    DevMsg(0, "[SPE] ==============================\n");
    return EXCEPTION_EXECUTE_HANDLER;
}