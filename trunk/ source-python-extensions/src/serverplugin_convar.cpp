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


#include <icvar.h>
#include <convar.h>

// memdbgon must be the last include file in a .cpp file!!!
#include <tier0/memdbgon.h>

static ICvar *s_pCVar;

class CPluginConVarAccessor : public IConCommandBaseAccessor
{
public:
	virtual bool	RegisterConCommandBase( ConCommandBase *pCommand )
	{
#if defined ORANGE_BOX
		pCommand->AddFlags( FCVAR_NONE );
#else
		pCommand->AddFlags( FCVAR_PLUGIN );

		// Unlink from plugin only list
		pCommand->SetNext( 0 );

		// Link to engine's list instead
		s_pCVar->RegisterConCommandBase( pCommand );
#endif
		return true;
	}

};

CPluginConVarAccessor g_ConVarAccessor;

void InitCVars( CreateInterfaceFn cvarFactory )
{
#if defined ORANGE_BOX
	s_pCVar = (ICvar*)cvarFactory( CVAR_INTERFACE_VERSION, NULL );
#else
	s_pCVar = (ICvar*)cvarFactory( VENGINE_CVAR_INTERFACE_VERSION, NULL );
#endif

	if ( s_pCVar )
	{
#if defined ORANGE_BOX
		ConVar_Register(0, &g_ConVarAccessor);
#else
		ConCommandBaseMgr::OneTimeInit( &g_ConVarAccessor );
#endif
	}
}

