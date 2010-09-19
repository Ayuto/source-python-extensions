/**
* =============================================================================
* Source Python Extensions
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

#ifdef _WIN32
#pragma once
#endif

//=================================================================================
// Includes
//=================================================================================
#include "Python.h"
#include "eiface.h"
#include "filesystem.h"
#include "playerinfomanager.h"
#include "igameevents.h"
#include "IEngineTrace.h"
#include "sourcehook_impl.h"
#include "sourcehook.h"
#include "spe_dyncall.h"

//=================================================================================
// Some useful definitions
//=================================================================================
#define MAX_STRING_LEN 2048

//=================================================================================
// External interfaces that this plugin will use.
//=================================================================================
extern IVEngineServer				*engine;
extern IFileSystem					*filesystem;
extern IGameEventManager2			*gameeventmanager;
extern IPlayerInfoManager			*playerinfomanager;
extern IEngineTrace					*enginetrace;
extern CGlobalVars					*gpGlobals;

//=================================================================================
// Global variables.
//=================================================================================
extern DCCallVM						*vm;
extern void							*laddr;

//=================================================================================
// SourceHook related variables.
//=================================================================================
using namespace						SourceHook;
extern SourceHook::ISourceHook	   *g_SHPtr;
extern SourceHook::CSourceHookImpl  g_SourceHook;
extern int						    g_PLID;

//=================================================================================
// Useful helper func.
//=================================================================================
inline bool FStrEq(const char *sz1, const char *sz2)
{
	return(Q_stricmp(sz1, sz2) == 0);
}

//=================================================================================
// Need these for L4D.
//=================================================================================
inline int IndexOfEdict(const edict_t *pEdict)
{
#if( ENGINE_VERSION >= 3 )
	return (int)(pEdict - gpGlobals->baseEdict);
#else
	return (int)engine->IndexOfEdict(pEdict);
#endif
}

inline edict_t *PEntityOfEntIndex(int iEntIndex)
{
	if (iEntIndex >= 0 && iEntIndex < gpGlobals->maxEntities)
	{
#if( ENGINE_VERSION >= 3 )
		return (edict_t *)(gpGlobals->baseEdict + iEntIndex);
#else
		return (edict_t *)(engine->PEntityOfEntIndex(iEntIndex));
#endif
	}
	return NULL;
}

#endif // SPE_GLOBALS_H
