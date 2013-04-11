//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
// $NoKeywords: $
//
//=============================================================================//

#include <icvar.h>
#include <convar.h>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

static ICvar *s_pCVar;

class CPluginConVarAccessor : public IConCommandBaseAccessor
{
public:
    virtual bool    RegisterConCommandBase( ConCommandBase *pCommand )
    {
#if( ENGINE_VERSION >= 2 )
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
#if( ENGINE_VERSION >= 2 )
    s_pCVar = (ICvar *)cvarFactory( CVAR_INTERFACE_VERSION, NULL );
#else
    s_pCVar = (ICvar*)cvarFactory( VENGINE_CVAR_INTERFACE_VERSION, NULL );
#endif
    if ( s_pCVar )
    {
#if( ENGINE_VERSION >= 2 )
        ConVar_Register(0, &g_ConVarAccessor);
#else
        ConCommandBaseMgr::OneTimeInit( &g_ConVarAccessor );
#endif
    }
}

