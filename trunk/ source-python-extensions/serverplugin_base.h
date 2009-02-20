//=============================================================================//
// Copyright: Deniz Sezen
//		Derived from the work of VALVe Software
//=============================================================================//

#ifndef SERVERPLUGIN_BASE
#define SERVERPLUGIN_BASE

/* YOUR PLUGIN NAME HERE! */
#define PLUGIN_NAME "Eventscripts Extensions"

/* YOUR PLUGIN AUTHOR HERE */
#define PLUGIN_AUTHOR "your-name-here"

/* YOUR PLUGIN VERSION HERE */
#define PLUGIN_VERSION "1.0.0"

#include <stdio.h>

#include <interface.h>
#include <filesystem.h>
#include <engine/iserverplugin.h>
#include <dlls/iplayerinfo.h>
#include <eiface.h>
#include <igameevents.h>
#include <convar.h>
#include <Color.h>
#include <vstdlib/random.h>
#include <engine/IEngineTrace.h>

// memdbgon must be the last include file in a .cpp file!!!
#include <tier0/memdbgon.h>

//Convar initialization
void InitCVars( CreateInterfaceFn cvarFactory );

// useful helper func
inline bool FStrEq(const char *sz1, const char *sz2)
{
	return(Q_stricmp(sz1, sz2) == 0);
}

//---------------------------------------------------------------------------------
// Purpose: a sample 3rd party plugin class
//---------------------------------------------------------------------------------
class CEmptyServerPlugin: public IServerPluginCallbacks, public IGameEventListener
{
public:
	CEmptyServerPlugin();
	~CEmptyServerPlugin();

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
	virtual PLUGIN_RESULT	ClientCommand( edict_t *pEntity );
	virtual PLUGIN_RESULT	NetworkIDValidated( const char *pszUserName, const char *pszNetworkID );

	// IGameEventListener Interface
	virtual void FireGameEvent( KeyValues * event );

	virtual int GetCommandIndex() { return m_iClientCommandIndex; }
private:
	int m_iClientCommandIndex;
};


#endif

