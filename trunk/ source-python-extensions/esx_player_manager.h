//=============================================================================//
// Copyright: Deniz Sezen
//=============================================================================//

#ifndef ESX_PLAYER_MANAGER_H
#define ESX_PLAYER_MANAGER_H

#include <eiface.h>
#include <iplayerinfo.h>

class CPlayerManager
{
	private:
		IVEngineServer* m_Engine;
		IPlayerInfoManager* m_Info;

	public:
		/* Constructor and Destructor */
		CPlayerManager( IVEngineServer* pEngine, IPlayerInfoManager* pInfo );

		/* Returns the edict of a player */
		edict_t* GetPlayerByUserID( int userid );
};

extern CPlayerManager* gPlayerManager;

#endif

