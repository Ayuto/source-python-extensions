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

#ifndef ESX_PLAYER_MANAGER_H
#define ESX_PLAYER_MANAGER_H

#include <eiface.h>
#include "iplayerinfo.h"

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

