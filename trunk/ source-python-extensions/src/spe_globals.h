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

#ifndef SPE_GLOBALS_H
#define SPE_GLOBALS_H

/* Includes */
#include <eiface.h>
#include <iplayerinfo.h>
#include <ivoiceserver.h>
#include <filesystem.h>
#include <igameevents.h>
#include <IEngineTrace.h>
#include "spe_dyncall_py.h"

//==================================================================================
// >> Global Variables
//==================================================================================

/* Interfaces from the engine */
extern IVEngineServer		*engine; 
extern IFileSystem			*filesystem; 
extern IGameEventManager2	*gameeventmanager; 
extern IPlayerInfoManager	*playerinfomanager;
extern CGlobalVars			*gpGlobals;
extern IEngineTrace			*enginetrace;

/* Custom global variables */
extern void					*laddr;
extern DCCallVM				*vm;

//==================================================================================
// >> Global Macros
//==================================================================================
//Does error handling for us
#define SPE_PY_EXCEPTION( reason ) \
	Msg( reason ); \
	PyErr_SetString(NULL, reason ); \
	return Py_BuildValue("");

#endif
