//=============================================================================//
// Copyright: Deniz Sezen
//=============================================================================//

#include "esx_player_manager.h"

CPlayerManager* gPlayerManager;

//=============================================================================//
// >> Constructor
//=============================================================================//
CPlayerManager::CPlayerManager( IVEngineServer *pEngine, IPlayerInfoManager* pInfo )
{
	m_Engine = pEngine;
	m_Info = pInfo;
}

//=============================================================================//
// >> Returns an edict_t to a player
//=============================================================================//
edict_t* CPlayerManager::GetPlayerByUserID(int userid)
{
	for(int i = 0; i < m_Info->GetGlobalVars()->maxClients; i++)
	{
		edict_t* player = m_Engine->PEntityOfEntIndex(i);

		if(!player || player->IsFree())
			continue;

		if(m_Engine->GetPlayerUserId(player) == -1)
			continue;

		if(m_Engine->GetPlayerUserId(player) == userid)
			return player;
	}

	return NULL;
}

