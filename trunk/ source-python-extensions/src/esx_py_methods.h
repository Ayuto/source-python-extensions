/**
* =============================================================================
* Eventscripts Extensions
* Copyright (C) 2009 Deniz "your-name-here" Sezen.  All rights reserved.
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

#ifndef ESX_PY_METHODS_H
#define ESX_PY_METHODS_H

#ifdef _WIN32
#pragma once
#endif

/* Includes */
#include <Python.h>

/* Methods */
PyObject* esx_GetPlayerPtr( PyObject* self, PyObject* args );
PyObject* esx_FindFuncPtr( PyObject* self, PyObject* args );
PyObject* esx_FindSymbol( PyObject* self, PyObject* args );
PyObject* esx_CallFunction( PyObject* self, PyObject* args );
PyObject* esx_SetCallingConvention( PyObject* self, PyObject* args );
PyObject* esx_RipPointer( PyObject* self, PyObject* args );
PyObject* esx_MutePlayer( PyObject* self, PyObject* args);
PyObject* esx_UnMutePlayer( PyObject* self, PyObject* args );
PyObject* esx_GetGameRules( PyObject* self, PyObject* args );


#endif
