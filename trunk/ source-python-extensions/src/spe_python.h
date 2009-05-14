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


#ifndef __spe_PYTHON_H__
#define __spe_PYTHON_H__

/* Includes */
#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif 
#include <Python.h>
#include "spe_dyncall_py.h"
#include "spe_py_methods.h"

/* Method table */
static PyMethodDef speMethods[] = {
	// Player related functions
	{ "getPlayer", spe_GetPlayerPtr, METH_VARARGS, "Returns a pointer to a player by userid." },

	// Binary related functions.
	{ "findFunction", spe_FindFuncPtr, METH_VARARGS, "Returns a pointer to a function by signature." },
	{ "findSymbol", spe_FindSymbol, METH_VARARGS, "Finds the address of a symbol in linux." },
	{ "callFunction", spe_CallFunction, METH_VARARGS, "Calls a function through the use of a CObject." },
	{ "ripPointer", spe_RipPointer, METH_VARARGS, "Rips a pointer from another pointer + an offset." },
	{ "setCallingConvention", spe_SetCallingConvention, METH_VARARGS, "Sets the CallVM calling convention." },
	{ "getVfnAddress", spe_GetVfnAddress, METH_VARARGS, "Gets the address of a virtual function." },

	// Hooking related functions.
	{ "registerPreHook", spe_PreHookEvent, METH_VARARGS, "Registers a prehook." },
	{ "unregisterPreHook", spe_UnHookEvent, METH_VARARGS, "Removes a prehook for an event." },


	{ NULL, NULL, 0, NULL }
};

/* Python initialization Function */
bool initializePython( char *pGameDir )
{

#ifdef _WIN32
	char pAddonDir[2047];
	V_snprintf( pAddonDir, 2047, "%s\\addons\\eventscripts", pGameDir );

	/* Load the libraries */
	HMODULE pPythonDLL = NULL;
	HMODULE pMSVCRDLL = NULL;

	char pDLL1[2047];
	char pDLL2[2047];

	V_snprintf( pDLL1, 2047, "%s\\_engines\\python\\Lib\\plat-win\\msvcr71.dll", pAddonDir );
	V_snprintf( pDLL2, 2047, "%s\\_engines\\python\\Lib\\plat-win\\python25.dll", pAddonDir );

	pMSVCRDLL = LoadLibrary( pDLL1 );
	pPythonDLL = LoadLibrary( pDLL2 );

	if( !pPythonDLL || !pMSVCRDLL )
	{
		DevMsg("[SPE] An error occured. Could not find msvcr71.dll or python25.DLL.\n");
		return false;
	}

#else
	if( !dlopen("libpython2.5.so.1.0", RTLD_NOW | RTLD_GLOBAL) )
	{
		Msg("[SPE] Unable to open libpython2.5.so.1.0!\n");
		return true;
	}
#endif

	Py_Initialize();

	/* Initialize the module */
	Py_InitModule( "esx_C", speMethods );
	
	return true;
}

#endif
