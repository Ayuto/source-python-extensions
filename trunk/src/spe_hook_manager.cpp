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

// -----------------------------------
// SPE includes
// -----------------------------------
#include "spe_python.h"
#include "spe_callback.h"
#include "spe_globals.h"

// -----------------------------------
// Dyndetours includes
// -----------------------------------
#include "dyndetours/detour_class.h"
#include "dyndetours/detourman_class.h"

//=================================================================================
// Adds a callback.
//=================================================================================
DECLARE_PYCMD( hookFunction, "Detours a function." )
{
    /* Parse out the python callback and the address. */
    PyObject* pyCallback = NULL;
    void*     pAddr      = NULL;
    char*     szParms    = NULL;
    eCallConv eConv;
    eHookType eType;

    if( !PyArg_ParseTuple(args, "isiiO", &pAddr, &szParms, &eConv, &eType, &pyCallback) )
    {
        DevMsg(1, "[SPE] Could not parse arguments for hookFunction!\n");
        return NULL;
    }

    /* Make sure the given address and callback are valid. */
    if( !pAddr || !pyCallback || !szParms ) {
        DevMsg(1, "[SPE] One or more arguments are invalid for hookFunction!\n");
        return NULL;
    }

    /* Create (or return) a/the detour at this location. */
    DevMsg("Stage 1: %d %s %d\n", pAddr, szParms, eConv);
    CDetour* pDetour = g_DetourManager.Add_Detour( pAddr, szParms, eConv );

    /* Does this detour instance contain a python callback handler? */
    DevMsg("Adding callback.\n");
    ICallbackManager* pCBM = pDetour->GetManager("py", eType);

    /* If not, we need to create one and add it to the detour. */
    if( !pCBM ) {
        pCBM = new CPythonCallback();
        pDetour->AddManager(pCBM, eType);
    }

    /* Add the python function to this callback. */
    pCBM->Add((void *)pyCallback, eType);

    /* Done. */
    return Py_BuildValue("");
}

//=================================================================================
// Removes a callback from the list.
//=================================================================================
DECLARE_PYCMD( unHookFunction, "Removes a python function callback from a detour." )
{
    /* Only need the address and python object. */
    PyObject* pyCallback = NULL;
    void*     pAddr      = NULL;
    eHookType eType;

    if( !PyArg_ParseTuple(args, "iiO", &pAddr, &eType, &pyCallback) )
    {
        DevMsg(1, "[SPE] Could not parse arguments for unHookFunction.");
        return NULL;
    }

    /* Get the detour instance at this address. */
    CDetour* pDetour = g_DetourManager.Find_Detour(pAddr);

    /* Did we find it? */
    if( !pDetour ) {
        DevMsg(1, "[SPE] Could not find detour at %d!\n", pAddr);
        return NULL;
    }

    /* Get the python callback manager (if there is one). */
    ICallbackManager* pCBM = pDetour->GetManager("py", eType);

    /* Did we find one? */
    if( !pCBM ) {
        DevMsg(1, "[SPE] Could not find a python callback manager.\n");
        return NULL;
    }

    /* Remove the python object. */
    pCBM->Remove(pyCallback, eType);

    /* Done. */
    return Py_BuildValue("");
}

//=================================================================================
// Returns the address of the trampoline of a hooked function.
//=================================================================================
DECLARE_PYCMD( getTrampoline, "Returns the address of the trampoline of a hooked function." )
{
    void* pAddr = NULL;
    if (!PyArg_ParseTuple(args, "i", &pAddr))
    {
        DevMsg("[SPE] Could not parse arguments for getTrampoline.\n");
        return NULL;
    }

    CDetour* pDetour = g_DetourManager.Find_Detour(pAddr);
    if (!pDetour)
    {
        DevMsg("[SPE] Could not find detour at %d!\n", pAddr);
        return NULL;
    }

    void* pTrampoline = pDetour-> GetTrampoline();
    return Py_BuildValue("i", pTrampoline);
}