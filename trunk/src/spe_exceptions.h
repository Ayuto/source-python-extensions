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

#ifndef SPE_EXCEPTIONS_H
#define SPE_EXCEPTIONS_H

//=================================================================================
// Includes
//=================================================================================
#ifdef _WIN32
    #include <Windows.h>


//=================================================================================
// MACROS
//=================================================================================
#define START_CATCH_EXC __try {
#define END_CATCH_EXC } __except(exceptionHandler(GetExceptionInformation(), GetExceptionCode())) {}


//=================================================================================
// This function will be called, when a critical error occurs
//=================================================================================
int exceptionHandler(EXCEPTION_POINTERS* info, unsigned long code);
#else
// Not implemented at the moment
#define START_CATCH_EXC
#define END_CATCH_EXC
#endif

#endif // SPE_EXCEPTIONS_H