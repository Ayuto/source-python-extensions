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
#include "spe_python.h"
#include <edict.h>

//=================================================================================
// Prevents the 'GetClassNameA' : is not a member of 'edict_t' error :/
//=================================================================================
#ifdef _WIN32
    #undef GetClassName
#endif

//=================================================================================
// Converts a CBaseEntity instance to an edict.
// Courtesy of the SourceMod Team.
//=================================================================================
edict_t* BaseEntityToEdict( CBaseEntity* pEntity )
{
    IServerUnknown *pUnk = (IServerUnknown *)pEntity;
    IServerNetworkable *pNet = pUnk->GetNetworkable();

    if( !pNet )
        return NULL;

    return pNet->GetEdict();
}

//=================================================================================
// Returns the index of an entity.
//=================================================================================
DECLARE_PYCMD( getEntityIndex, "Returns the index of an entity." )
{
    CBaseEntity* pEnt   = NULL;
    edict_t* pEdict     = NULL;

    if( !PyArg_ParseTuple(args, "i", &pEnt) )
    {
        DevMsg("[SPE] spe_getEntityIndex: The arguments could not be parsed.\n");
        return Py_BuildValue("");
    }

    if( !pEnt )
    {
        DevMsg("[SPE] spe_getEntityIndex: Invalid entity passed in!\n");
        return Py_BuildValue("");
    }

    pEdict = BaseEntityToEdict( pEnt );

    if( !pEdict )
    {
        DevMsg("[SPE] spe_getEntityIndex: Could not convert entity to edict!\n");
        return Py_BuildValue("");
    }

    return Py_BuildValue("i", IndexOfEdict(pEdict));
}

//=================================================================================
// Returns the classname of a particular edict.
// Courtesy of the SourceMod Team.
//=================================================================================
DECLARE_PYCMD( getEntityClassName, "Returns the classname of an entity." )
{
    CBaseEntity* pEnt   = NULL;
    edict_t*     pEdict = NULL;
    const char*  szClassName = NULL;

    if( !PyArg_ParseTuple(args, "i", &pEnt) )
    {
        DevMsg("[SPE] spe_getPlayer: The arguments could not be parsed.\n");
        return Py_BuildValue("");
    }

    if( !pEnt )
    {
        DevMsg("[SPE] spe_getEntityClassName: Invalid entity passed in!\n");
        return Py_BuildValue("");
    }

    pEdict = BaseEntityToEdict( pEnt );

    if( !pEdict )
    {
        DevMsg("[SPE] spe_getEntityClassName: Could not convert entity to edict!\n");
        return Py_BuildValue("");
    }

    szClassName = pEdict->GetClassName();

    if( !szClassName || szClassName[0] == '\0' )
    {
        DevMsg("[SPE] spe_getEntityClassName: Classname was either null or invalid.\n");
        return Py_BuildValue("");
    }

    return Py_BuildValue("s", szClassName);
}