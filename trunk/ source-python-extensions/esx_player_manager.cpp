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

