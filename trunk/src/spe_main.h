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

#ifndef SPE_MAIN_H
#define SPE_MAIN_H

#ifdef _WIN32
#pragma once
#endif

/* YOUR PLUGIN NAME HERE! */
#define PLUGIN_NAME "Source Python Extensions"

/* YOUR PLUGIN AUTHOR HERE */
#define PLUGIN_AUTHOR "your-name-here"

/* YOUR PLUGIN VERSION HERE */
#define PLUGIN_VERSION "1.5.0f"

//=================================================================================
// Includes
//=================================================================================
#include "engine/iserverplugin.h"
#include "strtools.h"
#include "igameevents.h"

#if( ENGINE_VERSION >= 2 )
#include <tier2/tier2.h>
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//=================================================================================
// Useful helper func.
//=================================================================================
inline bool FStrEq(const char *sz1, const char *sz2)
{
	return(Q_stricmp(sz1, sz2) == 0);
}

//=================================================================================
// Source Python Extensions plugin class.
//=================================================================================
class CSPE_Plugin: public IServerPluginCallbacks, public IGameEventListener2
{
public:
	CSPE_Plugin();
	~CSPE_Plugin();

	// IServerPluginCallbacks methods
	virtual bool			Load(	CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory );
	virtual void			Unload( void );
	virtual void			Pause( void );
	virtual void			UnPause( void );
	virtual const char     *GetPluginDescription( void );      
	virtual void			LevelInit( char const *pMapName );
	virtual void			ServerActivate( edict_t *pEdictList, int edictCount, int clientMax );
	virtual void			GameFrame( bool simulating );
	virtual void			LevelShutdown( void );
	virtual void			ClientActive( edict_t *pEntity );
	virtual void			ClientDisconnect( edict_t *pEntity );
	virtual void			ClientPutInServer( edict_t *pEntity, char const *playername );
	virtual void			SetCommandClient( int index );
	virtual void			ClientSettingsChanged( edict_t *pEdict );
	virtual PLUGIN_RESULT	ClientConnect( bool *bAllowConnect, edict_t *pEntity, const char *pszName, const char *pszAddress, char *reject, int maxrejectlen );

#if( ENGINE_VERSION >= 2 )
	virtual PLUGIN_RESULT	ClientCommand(edict_t* pEdict, const CCommand &args);
	void					OnQueryCvarValueFinished( QueryCvarCookie_t iCookie, edict_t *pPlayerEntity, EQueryCvarValueStatus eStatus, const char *pCvarName, const char *pCvarValue );
#else
	virtual PLUGIN_RESULT	ClientCommand( edict_t *pEntity );
#endif
	virtual PLUGIN_RESULT	NetworkIDValidated( const char *pszUserName, const char *pszNetworkID );

	// IGameEventListener Interface
	virtual void FireGameEvent( IGameEvent * event );

	virtual int GetCommandIndex() { return m_iClientCommandIndex; }
private:
	int m_iClientCommandIndex;
};

#endif // SPE_MAIN_H
