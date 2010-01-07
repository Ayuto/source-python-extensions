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
#include "spe_python.h"

//=================================================================================
// Need these for L4D.
//=================================================================================
#if( ENGINE_VERSION >= 3 )
	inline int IndexOfEdict(const edict_t *pEdict)
	{
		return (int)(pEdict - gpGlobals->baseEdict);
	}

	inline edict_t *PEntityOfEntIndex(int iEntIndex)
	{
		if (iEntIndex >= 0 && iEntIndex < gpGlobals->maxEntities)
		{
			return (edict_t *)(gpGlobals->baseEdict + iEntIndex);
		}
		return NULL;
	}
#endif 

//=================================================================================
// Returns a player instance as an int.
//=================================================================================
DECLARE_PYCMD( getPlayer, "Returns the address of the instance of the player." )
{
	if( !engine )
	{
		Error("[SPE]: The engine instance was not valid.\n");
		return Py_BuildValue("");
	}

	int userid;
	if( !PyArg_ParseTuple(args, "i", &userid) )
	{
		DevMsg("[SPE]: spe_getPlayer: The arguments could not be parsed.\n");
		return Py_BuildValue("");
	}

	// Get the player by their userid.
	edict_t *pPlayer = NULL;
	for(int i = 0; i < playerinfomanager->GetGlobalVars()->maxClients; i++)
	{
#if( ENGINE_VERSION >= 3 )
		edict_t* player = PEntityOfEntIndex(i);
#else
		edict_t* player = engine->PEntityOfEntIndex(i);
#endif

		if(!player || player->IsFree())
			continue;

		if(engine->GetPlayerUserId(player) == -1)
			continue;

		if(engine->GetPlayerUserId(player) == userid)
			return Py_BuildValue("i", (int)player->GetUnknown()->GetBaseEntity());
	}

	// If we are here, we didn't find a player with that userid.
	DevMsg("[SPE]: spe_getPlayer: Could not find a player with the userid of %d.\n", userid);
	return Py_BuildValue("");
}
