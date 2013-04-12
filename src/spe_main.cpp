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
#include "spe_main.h"
#include "spe_globals.h"
#include "spe_python.h"
#include "svn_build.h"
#include "playerinfomanager.h"


#ifdef _LINUX
#include <dlfcn.h>
#else
#include <windows.h>
#endif

//=================================================================================
// Interface declarations
//=================================================================================
IVEngineServer* engine    = NULL;
CGlobalVars*    gpGlobals = NULL;
DCCallVM*       vm        = NULL;
void*           laddr     = NULL;

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
//          engine.
//=================================================================================
bool CSPE_Plugin::Load( CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory )
{
    Msg("[SPE] Loading Source-Python-Extensions " PLUGIN_VERSION ", r" SVN_WC_REVISION "...\n");
#if( ENGINE_VERSION >= 2 )
    ConnectTier1Libraries( &interfaceFactory, 1 );
    ConnectTier2Libraries( &interfaceFactory, 1 );
    ConVar_Register( 0 );
#else
    InitCVars( interfaceFactory ); // register any cvars we have defined
#endif

    IPlayerInfoManager* playerinfomanager = (IPlayerInfoManager *) gameServerFactory(INTERFACEVERSION_PLAYERINFOMANAGER,NULL);
    if ( !playerinfomanager )
    {
        Msg("[SPE] Unable to load playerinfomanager.\n" );
        return false;
    }

    engine = (IVEngineServer*) interfaceFactory(INTERFACEVERSION_VENGINESERVER, NULL);
    if( !engine)
    {
        Msg("[SPE] Unable to load engine.\n");
        return false;
    }

    gpGlobals = playerinfomanager->GetGlobalVars();

    // Setup dyncall
    vm = dcNewCallVM(4026);

    if (!vm)
    {
        Msg("[SPE] Could not create a virtual machine.\n");
        return false;
    }

    // Get the game directory
    char szServerBinary[300];
    engine->GetGameDir( szServerBinary, 300 );

    // Add the bin dir
    strcat( szServerBinary, "/bin/server");

    // OS specific stuff
#ifdef _WIN32
    strcat( szServerBinary, ".dll" );
    laddr = (void *) LoadLibrary(szServerBinary);
#else
    strcat( szServerBinary, "_srv.so" );
    laddr = dlopen( szServerBinary, RTLD_NOW );
#endif

    if(!laddr)
    {
        Msg("[SPE] Failed to open server image.\n");
        return false;
    }

    // Initialize python
    if (!EnablePython())
        return false;

    Msg("[SPE] Loaded succesfully.\n");
    return true;
}

//=================================================================================
// Purpose: called when the plugin is unloaded (turned off)
//=================================================================================
void CSPE_Plugin::Unload( void )
{
    // Free up memory from dyncall.
    dcFree(vm);

#if( ENGINE_VERSION >= 2 )
    ConVar_Unregister();
    DisconnectTier2Libraries();
    DisconnectTier1Libraries();
#endif

#ifdef _LINUX
    dlclose(laddr);
#else
    FreeLibrary((HMODULE) laddr);
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
    return (PLUGIN_NAME ", "
            PLUGIN_DATE ", "
            PLUGIN_AUTHOR ", "
            PLUGIN_VERSION ", r"
            SVN_WC_REVISION);
}

//=================================================================================
// Purpose: called on level start
//=================================================================================
void CSPE_Plugin::LevelInit( char const *pMapName )
{

}

//=================================================================================
// Purpose: called on level start, when the server is ready to accept client connections
//      edictCount is the number of entities in the level, clientMax is the max client count
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
