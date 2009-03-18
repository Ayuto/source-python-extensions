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
#include "esx_globals.h"

CPlayerManager* gPlayerManager;

//=============================================================================
// >> Constructor
//=============================================================================
CPlayerManager::CPlayerManager()
{
	return;
}

//=============================================================================
// >> Returns an edict_t to a player
//=============================================================================
edict_t* CPlayerManager::GetPlayerByUserID(int userid)
{
	for(int i = 0; i < gGlobals->m_Info->GetGlobalVars()->maxClients; i++)
	{
		edict_t* player = gGlobals->m_Engine->PEntityOfEntIndex(i);

		if(!player || player->IsFree())
			continue;

		if(gGlobals->m_Engine->GetPlayerUserId(player) == -1)
			continue;

		if(gGlobals->m_Engine->GetPlayerUserId(player) == userid)
			return player;
	}

	return NULL;
}

//=============================================================================
// >> Mutes a player
//=============================================================================
void CPlayerManager::MutePlayer( int userid )
{
	//Get the person to be muted index
	edict_t* pPlayer = GetPlayerByUserID( userid );
	int iIndex = gGlobals->m_Engine->IndexOfEdict( pPlayer ) - 1;

	for( int i = 0; i < gGlobals->m_Info->GetGlobalVars()->maxClients; i++ )
	{
		//Now loop through everyone and mute them
		edict_t* pOtherPlayer = gGlobals->m_Engine->PEntityOfEntIndex( i );
		int iOtherIndex = gGlobals->m_Engine->IndexOfEdict( pOtherPlayer ) - 1;

		//Mute them
		gGlobals->m_Voice->SetClientListening( iOtherIndex, iIndex, false );
	}
}

//=============================================================================
// >> Unmutes a player
//=============================================================================
void CPlayerManager::UnMutePlayer( int userid )
{
	edict_t* pPlayer = GetPlayerByUserID( userid );

	int iIndex = gGlobals->m_Engine->IndexOfEdict( pPlayer ) - 1;

	for( int i = 0; i < gGlobals->m_Info->GetGlobalVars()->maxClients; i++ )
	{
		edict_t* pOtherPlayer = gGlobals->m_Engine->PEntityOfEntIndex( i );
		int iOtherIndex = gGlobals->m_Engine->IndexOfEdict( pOtherPlayer ) - 1;

		//Unmute them to this player
		gGlobals->m_Voice->SetClientListening( iOtherIndex, iIndex, true );
	}
}
