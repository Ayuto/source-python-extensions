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


#include "serverplugin_base.h"
#include "esx_player_manager.h"
#include "esx_signature_manager.h"
#include "esx_python.h"

// Interfaces from the engine
IVEngineServer		*engine = NULL; // helper functions (messaging clients, loading content, making entities, running commands, etc)
IFileSystem		*filesystem = NULL; // file I/O 
IGameEventManager 	*gameeventmanager = NULL; // game events interface
IPlayerInfoManager 	*playerinfomanager = NULL; // game dll interface to interact with player
IServerPluginHelpers 	*helpers = NULL; // special 3rd party plugin helpers from the engine
IUniformRandomStream 	*randomStr = NULL;
IEngineTrace 		*enginetrace = NULL;
CGlobalVars 		*gpGlobals = NULL;

// 
// The plugin is a static singleton that is exported as an interface
//
CEmptyServerPlugin g_EmtpyServerPlugin;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CEmptyServerPlugin, IServerPluginCallbacks, INTERFACEVERSION_ISERVERPLUGINCALLBACKS, g_EmtpyServerPlugin );

//---------------------------------------------------------------------------------
// Purpose: constructor/destructor
//---------------------------------------------------------------------------------
CEmptyServerPlugin::CEmptyServerPlugin()
{
	m_iClientCommandIndex = 0;
}

CEmptyServerPlugin::~CEmptyServerPlugin()
{
}

//---------------------------------------------------------------------------------
// Purpose: called when the plugin is loaded, load the interface we need from the engine
//---------------------------------------------------------------------------------
bool CEmptyServerPlugin::Load(	CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory )
{
	playerinfomanager = (IPlayerInfoManager *)gameServerFactory(INTERFACEVERSION_PLAYERINFOMANAGER,NULL);
	if ( !playerinfomanager )
	{
		Warning( "Unable to load playerinfomanager, ignoring\n" ); // this isn't fatal, we just won't be able to access specific player data
	}

	// get the interfaces we want to use
	if(	!(engine = (IVEngineServer*)interfaceFactory(INTERFACEVERSION_VENGINESERVER, NULL)) ||
		!(gameeventmanager = (IGameEventManager *)interfaceFactory(INTERFACEVERSION_GAMEEVENTSMANAGER,NULL)) ||
		!(filesystem = (IFileSystem*)interfaceFactory(FILESYSTEM_INTERFACE_VERSION, NULL)) ||
		!(helpers = (IServerPluginHelpers*)interfaceFactory(INTERFACEVERSION_ISERVERPLUGINHELPERS, NULL)) || 
		!(enginetrace = (IEngineTrace *)interfaceFactory(INTERFACEVERSION_ENGINETRACE_SERVER,NULL)) ||
		!(randomStr = (IUniformRandomStream *)interfaceFactory(VENGINE_SERVER_RANDOM_INTERFACE_VERSION, NULL))
		)
	{
		return false; // we require all these interface to function
	}

	if ( playerinfomanager )
	{
		gpGlobals = playerinfomanager->GetGlobalVars();
	}

	//Initialize some crap that VALVe wrote
	InitCVars( interfaceFactory ); // register any cvars we have defined

	/* Initialize our classes */
	gPlayerManager = new CPlayerManager( engine, playerinfomanager );
	gSigger = new CSigger( reinterpret_cast<void*>( gameServerFactory ) );

	char pGameDir[1024];
	engine->GetGameDir( pGameDir, 1024 );
	
	//initializePython( pGameDir );
	
	return true;
}

//---------------------------------------------------------------------------------
// Purpose: called when the plugin is unloaded (turned off)
//---------------------------------------------------------------------------------
void CEmptyServerPlugin::Unload( void )
{
	gameeventmanager->RemoveListener( this ); // make sure we are unloaded from the event system
	Py_Finalize();
}

//---------------------------------------------------------------------------------
// Purpose: called when the plugin is paused (i.e should stop running but isn't unloaded)
//---------------------------------------------------------------------------------
void CEmptyServerPlugin::Pause( void )
{
}

//---------------------------------------------------------------------------------
// Purpose: called when the plugin is unpaused (i.e should start executing again)
//---------------------------------------------------------------------------------
void CEmptyServerPlugin::UnPause( void )
{
}

//---------------------------------------------------------------------------------
// Purpose: the name of this plugin, returned in "plugin_print" command
//---------------------------------------------------------------------------------
const char *CEmptyServerPlugin::GetPluginDescription( void )
{
	return "Eventscripts Extensions, your-name-here, 2009";
}

//---------------------------------------------------------------------------------
// Purpose: called on level start
//---------------------------------------------------------------------------------
void CEmptyServerPlugin::LevelInit( char const *pMapName )
{
	gameeventmanager->AddListener( this, true );
}

//---------------------------------------------------------------------------------
// Purpose: called on level start, when the server is ready to accept client connections
//		edictCount is the number of entities in the level, clientMax is the max client count
//---------------------------------------------------------------------------------
void CEmptyServerPlugin::ServerActivate( edict_t *pEdictList, int edictCount, int clientMax )
{
}

//---------------------------------------------------------------------------------
// Purpose: called once per server frame, do recurring work here (like checking for timeouts)
//---------------------------------------------------------------------------------
void CEmptyServerPlugin::GameFrame( bool simulating )
{
}

//---------------------------------------------------------------------------------
// Purpose: called on level end (as the server is shutting down or going to a new map)
//---------------------------------------------------------------------------------
void CEmptyServerPlugin::LevelShutdown( void ) // !!!!this can get called multiple times per map change
{
	gameeventmanager->RemoveListener( this );
}

//---------------------------------------------------------------------------------
// Purpose: called when a client spawns into a server (i.e as they begin to play)
//---------------------------------------------------------------------------------
void CEmptyServerPlugin::ClientActive( edict_t *pEntity )
{
}

//---------------------------------------------------------------------------------
// Purpose: called when a client leaves a server (or is timed out)
//---------------------------------------------------------------------------------
void CEmptyServerPlugin::ClientDisconnect( edict_t *pEntity )
{
}

//---------------------------------------------------------------------------------
// Purpose: called on 
//---------------------------------------------------------------------------------
void CEmptyServerPlugin::ClientPutInServer( edict_t *pEntity, char const *playername )
{
}

//---------------------------------------------------------------------------------
// Purpose: called on level start
//---------------------------------------------------------------------------------
void CEmptyServerPlugin::SetCommandClient( int index )
{
}

//---------------------------------------------------------------------------------
// Purpose: called on level start
//---------------------------------------------------------------------------------
void CEmptyServerPlugin::ClientSettingsChanged( edict_t *pEdict )
{
}

//---------------------------------------------------------------------------------
// Purpose: called when a client joins a server
//---------------------------------------------------------------------------------
PLUGIN_RESULT CEmptyServerPlugin::ClientConnect( bool *bAllowConnect, edict_t *pEntity, const char *pszName, const char *pszAddress, char *reject, int maxrejectlen )
{
	return PLUGIN_CONTINUE;
}

//---------------------------------------------------------------------------------
// Purpose: called when a client types in a command (only a subset of commands however, not CON_COMMAND's)
//---------------------------------------------------------------------------------
PLUGIN_RESULT CEmptyServerPlugin::ClientCommand( edict_t *pEntity )
{
	/*const char *pcmd = engine->Cmd_Argv(0);

	if ( !pEntity || pEntity->IsFree() ) 
	{
		return PLUGIN_CONTINUE;
	}*/

	return PLUGIN_CONTINUE;
}

//---------------------------------------------------------------------------------
// Purpose: called when a client is authenticated
//---------------------------------------------------------------------------------
PLUGIN_RESULT CEmptyServerPlugin::NetworkIDValidated( const char *pszUserName, const char *pszNetworkID )
{
	return PLUGIN_CONTINUE;
}

//---------------------------------------------------------------------------------
// Purpose: called when an event is fired
//---------------------------------------------------------------------------------
void CEmptyServerPlugin::FireGameEvent( KeyValues * event )
{
	//const char * name = event->GetName();
}

//---------------------------------------------------------------------------------
// Purpose: an example of how to implement a new command
//---------------------------------------------------------------------------------
CON_COMMAND( empty_version, "prints the version of the empty plugin" )
{
	Msg( PLUGIN_VERSION );
}

CON_COMMAND( empty_log, "logs the version of the empty plugin" )
{
	engine->LogPrint( PLUGIN_VERSION );
}

//---------------------------------------------------------------------------------
// Purpose: an example cvar
//---------------------------------------------------------------------------------
static ConVar empty_cvar("esx_version", "1.0a", 0, "Version of Eventscripts Extensions");
