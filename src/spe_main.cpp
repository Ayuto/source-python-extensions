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

//=================================================================================
// Includes
//=================================================================================
#include "spe_main.h"
#include "spe_globals.h"
#include "spe_python.h"
#include "spe_hook_manager.h"
#include "spe_event_parser.h"
#include "svn_build.h"

#ifdef _LINUX
#include <dlfcn.h>
#else
#include <windows.h>
#endif

//=================================================================================
// Interface declarations
//=================================================================================
IVEngineServer				*engine				= NULL;
IFileSystem					*filesystem			= NULL;
IGameEventManager2			*gameeventmanager	= NULL; 
IPlayerInfoManager			*playerinfomanager	= NULL;
IEngineTrace				*enginetrace		= NULL;
CGlobalVars					*gpGlobals			= NULL;
DCCallVM					*vm					= NULL;
void						*laddr				= NULL;

//////////////////////////////////////////////////////////////////////////
// Sourcehook variables
//////////////////////////////////////////////////////////////////////////
SourceHook::ISourceHook				*g_SHPtr;
SourceHook::CSourceHookImpl	 g_SourceHook;
int									 g_PLID;

//=================================================================================
// Function to initialize any cvars/command in this plugin
//=================================================================================
void InitCVars( CreateInterfaceFn cvarFactory );

//=================================================================================
// The plugin is a static singleton that is exported as an interface
//=================================================================================
CSPE_Plugin g_SPEPlugin;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CSPE_Plugin, IServerPluginCallbacks, INTERFACEVERSION_ISERVERPLUGINCALLBACKS, g_SPEPlugin);

//=================================================================================
// Purpose: Source Python Extensions version variable.
//=================================================================================
static ConVar spe_version_var("spe_version_var", "", 0, "Version of Source Python Extensions.");

//=================================================================================
// Purpose: Source Python Extensions engine variable
// TODO: Just set this to engine version.
//=================================================================================
#if( ENGINE_VERSION == 1 )
static ConVar spe_engine_version("spe_engine", "ep1", 0);
#elif( ENGINE_VERSION == 2 )
static ConVar spe_engine_version("spe_engine", "ep2", 0);
#elif ( ENGINE_VERSION == 3 )
static ConVar spe_engine_version("spe_engine", "l4d", 0);
#elif ( ENGINE_VERSION == 4 )
static ConVar spe_engine_version("spe_engine", "l4d2", 0);
#endif

//=================================================================================
// Purpose: constructor/destructor
//=================================================================================
CSPE_Plugin::CSPE_Plugin()
{
	m_iClientCommandIndex = 0;
}

CSPE_Plugin::~CSPE_Plugin()
{

}

//=================================================================================
// Purpose: called when the plugin is loaded, load the interface we need from the 
//			engine.
//=================================================================================
bool CSPE_Plugin::Load(	CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory )
{
#if( ENGINE_VERSION >= 2 )
	ConnectTier1Libraries( &interfaceFactory, 1 );
	ConnectTier2Libraries( &interfaceFactory, 1 );
	ConVar_Register( 0 );
#else
	InitCVars( interfaceFactory ); // register any cvars we have defined
#endif

	playerinfomanager = (IPlayerInfoManager *)gameServerFactory(INTERFACEVERSION_PLAYERINFOMANAGER,NULL);
	if ( !playerinfomanager )
	{
		DevMsg( "Unable to load playerinfomanager, ignoring\n" ); // this isn't fatal, we just won't be able to access specific player data
	}

	// get the interfaces we want to use
	if(	!(engine = (IVEngineServer*)interfaceFactory(INTERFACEVERSION_VENGINESERVER, NULL)) ||
		!(gameeventmanager = (IGameEventManager2 *)interfaceFactory(INTERFACEVERSION_GAMEEVENTSMANAGER2,NULL)) ||
		!(filesystem = (IFileSystem*)interfaceFactory(FILESYSTEM_INTERFACE_VERSION, NULL)) ||
		!(enginetrace = (IEngineTrace *)interfaceFactory(INTERFACEVERSION_ENGINETRACE_SERVER,NULL))
		)
	{
		return false; // we require all these interface to function
	}

	if ( playerinfomanager )
	{
		gpGlobals = playerinfomanager->GetGlobalVars();
	}

	// Setup dyncall
	vm = dcNewCallVM(4026);

	// Get the game directory
	char szServerBinary[300];
	engine->GetGameDir( szServerBinary, 300 );

	// Add the bin dir
	strcat( szServerBinary, "/bin/server");

	// OS specific stuff
#ifdef _WIN32
	
	// Use DLL extension
	strcat( szServerBinary, ".dll" );

	// Load library the handle
	laddr = (void *)LoadLibrary(szServerBinary);

#else 

	// Use _i486.so extension
	strcat( szServerBinary, "_i486.so" );

	// dlopen the library
    laddr = dlopen( szServerBinary, RTLD_NOW );

    if( laddr == NULL )
    {
        DevMsg("[SPE]: Failed to open server image.\n");
        return false;
    }

    Msg("[SPE]: Handle address is %d.\n", laddr);

#endif

	// Setup sourcehook
	g_SHPtr = &g_SourceHook;
	g_PLID = 0;

	// Setup the event parser
	g_pParser = new CModEventParser();

	// Setup the hook manager.
	gpHookMan = new CSPEHookManager( gameeventmanager );

	// Print all this information to a string
	char szVersionInfo[1024];
	Q_snprintf( szVersionInfo, sizeof(szVersionInfo), "%s r%s", PLUGIN_VERSION, SVN_WC_REVISION );

	// Set our version variable to reflect this
	spe_version_var.SetValue( szVersionInfo );

	// Make the version variable public
	spe_version_var.AddFlags(FCVAR_REPLICATED | FCVAR_NOTIFY);

    // Initialize python
	return EnablePython();
}

//=================================================================================
// Purpose: called when the plugin is unloaded (turned off)
//=================================================================================
void CSPE_Plugin::Unload( void )
{
	// NOTE: Do we even listen to events?
	// gameeventmanager->RemoveListener( this );

	delete g_pParser;
	delete gpHookMan;
	dcFree(vm);

#if( ENGINE_VERSION >= 2 )
	ConVar_Unregister();
	DisconnectTier2Libraries();
	DisconnectTier1Libraries();
#endif

#ifdef _LINUX
    if( laddr )
	{
        dlclose(laddr);
	}
#endif
}

//=================================================================================
// Purpose: called when the plugin is paused (i.e should stop running but isn't unloaded)
//=================================================================================
void CSPE_Plugin::Pause( void )
{

}

//=================================================================================
// Purpose: called when the plugin is unpaused (i.e should start executing again)
//=================================================================================
void CSPE_Plugin::UnPause( void )
{

}

//=================================================================================
// Purpose: the name of this plugin, returned in "plugin_print" command
//=================================================================================
const char *CSPE_Plugin::GetPluginDescription( void )
{
	return "Source Python Extensions, 2009 - 2010, your-name-here";
}

//=================================================================================
// Purpose: called on level start
//=================================================================================
void CSPE_Plugin::LevelInit( char const *pMapName )
{

}

//=================================================================================
// Purpose: called on level start, when the server is ready to accept client connections
//		edictCount is the number of entities in the level, clientMax is the max client count
//=================================================================================
void CSPE_Plugin::ServerActivate( edict_t *pEdictList, int edictCount, int clientMax )
{
}

//=================================================================================
// Purpose: called once per server frame, do recurring work here (like checking for timeouts)
//=================================================================================
void CSPE_Plugin::GameFrame( bool simulating )
{
	
}

//=================================================================================
// Purpose: called on level end (as the server is shutting down or going to a new map)
//=================================================================================
void CSPE_Plugin::LevelShutdown( void )
{

}

//=================================================================================
// Purpose: called when a client spawns into a server (i.e as they begin to play)
//=================================================================================
void CSPE_Plugin::ClientActive( edict_t *pEntity )
{
}

//=================================================================================
// Purpose: called when a client leaves a server (or is timed out)
//=================================================================================
void CSPE_Plugin::ClientDisconnect( edict_t *pEntity )
{
}

//=================================================================================
// Purpose: called on 
//=================================================================================
void CSPE_Plugin::ClientPutInServer( edict_t *pEntity, char const *playername )
{

}

//=================================================================================
// Purpose: called on level start
//=================================================================================
void CSPE_Plugin::SetCommandClient( int index )
{
	m_iClientCommandIndex = index;
}

//=================================================================================
// Purpose: called on level start
//=================================================================================
void CSPE_Plugin::ClientSettingsChanged( edict_t *pEdict )
{
	
}

//=================================================================================
// Purpose: called when a client joins a server
//=================================================================================
PLUGIN_RESULT CSPE_Plugin::ClientConnect( bool *bAllowConnect, edict_t *pEntity, const char *pszName, const char *pszAddress, char *reject, int maxrejectlen )
{
	return PLUGIN_CONTINUE;
}

//=================================================================================
// Purpose: called when a client types in a command (only a subset of commands however, not CON_COMMAND's)
//=================================================================================
#if( ENGINE_VERSION >= 2 )
PLUGIN_RESULT CSPE_Plugin::ClientCommand(edict_t* pEdict, const CCommand &args)
#else
PLUGIN_RESULT CSPE_Plugin::ClientCommand( edict_t *pEntity )
#endif
{
	return PLUGIN_CONTINUE;
}

//=================================================================================
// Purpose: called when a client is authenticated
//=================================================================================
PLUGIN_RESULT CSPE_Plugin::NetworkIDValidated( const char *pszUserName, const char *pszNetworkID )
{
	return PLUGIN_CONTINUE;
}

//=================================================================================
// Purpose: called when an event is fired
//=================================================================================
void CSPE_Plugin::FireGameEvent( IGameEvent * event )
{

}

//=================================================================================
// Called when a cvar query value is finished
//=================================================================================
#if( ENGINE_VERSION >= 2 )
void CSPE_Plugin::OnQueryCvarValueFinished( QueryCvarCookie_t iCookie, edict_t *pPlayerEntity, 
										   EQueryCvarValueStatus eStatus, const char *pCvarName, 
										   const char *pCvarValue )
{
	// Do nothing
}
#endif

//=================================================================================
// Purpose: an example of how to implement a new command
//=================================================================================
CON_COMMAND( spe_version, "prints the version of the empty plugin" )
{
	char szInfo[1024];
	V_snprintf(szInfo, 1024, "%s, %s revision %s, %s\n", PLUGIN_NAME, PLUGIN_VERSION, 
			SVN_WC_REVISION, PLUGIN_AUTHOR);
	Msg(szInfo);
}
