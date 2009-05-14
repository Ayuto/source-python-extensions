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

#include "spe_player_manager.h"
#include "spe_py_methods.h"
#include "spe_signature_manager.h"
#include "spe_dyncall_py.h"
#include "spe_globals.h"
#include "spe_hook_manager.h"
#include "spe_call_manager.h"
#ifdef __linux__
#include <dlfcn.h>
#endif

//=============================================================================
// >> Returns a CObject to a userid.
//=============================================================================
PyObject* spe_GetPlayerPtr( PyObject* self, PyObject* args )
{
	int userid = 0;

	if( !PyArg_ParseTuple( args, "i", &userid ) )
	{
		SPE_PY_EXCEPTION( "[SPE] Could not parse tuple in spe_GetPlayerPtr!\n" );
	}

	void* ent = gPlayerManager->GetPlayerByUserID(userid)->GetUnknown()->GetBaseEntity();

	if( !ent )
	{
		SPE_PY_EXCEPTION( "[SPE] The entity was null in spe_GetPlayerPtr!\n" );
	}

	return PyCObject_FromVoidPtr( ent, NULL );
}

//=============================================================================
// >> Returns a CObject to a function, using a signature
//=============================================================================
PyObject* spe_FindFuncPtr( PyObject* self, PyObject* args )
{
	PyObject* p;
	int length = 0;

	if( !PyArg_ParseTuple( args, "Oi", &p, &length ) )
	{
		SPE_PY_EXCEPTION( "[SPE] Could not parse tuple in spe_FindfuncPtr!\n" );
	}

	void* addr = gSigger->findFunction( PyString_AsString(p), length );
	
	if( !addr )
	{
		SPE_PY_EXCEPTION( "[SPE] Error! Could not find signature!\n" )
	}
	
	Msg("[SPE] Address: %d\n", addr);

	return PyCObject_FromVoidPtr( addr , NULL );
}

//=============================================================================
// >> Sets the calling convention of the virtual machine
//=============================================================================
PyObject* spe_SetCallingConvention( PyObject* self, PyObject* args )
{
	const char* conv;

	if( !PyArg_ParseTuple( args, "s", &conv ) )
	{
		SPE_PY_EXCEPTION( "[SPE] Could not parse tuple in spe_SetCallingConvention!\n" )
	}

	dcReset( vm );

	if( strcmp( conv, "thiscall" ) == 0 )
	{
		#ifdef _WIN32
			dcMode( vm, DC_CALL_C_X86_WIN32_THIS_MS );
		#else
			dcMode( vm, DC_CALL_C_X86_WIN32_THIS_GNU );
		#endif
	}

	else if( strcmp( conv, "fastcall" ) == 0 )
	{
		#ifdef _WIN32
			dcMode( vm, DC_CALL_C_X86_WIN32_FAST_MS );
		#else
			dcMode( vm, DC_CALL_C_X86_WIN32_FAST_GNU);
		#endif
	}

	else if( strcmp( conv, "cdecl" ) == 0 )
	{
		dcMode( vm, DC_CALL_C_DEFAULT );
	}

	else
	{
		SPE_PY_EXCEPTION( "[SPE] Unknown calling convention type!\n" );
	}

	return Py_BuildValue("");
}

//=============================================================================
// >> Finds and returns the address of a function symbol on linux
//=============================================================================
PyObject* spe_FindSymbol( PyObject* self, PyObject* args )
{
	#ifdef _WIN32
		SPE_PY_EXCEPTION("[SPE]: You cannot use this function on windows!\n");
	#else
		/* Path to the game dir */
		char szGameDir[2048];
		gGlobals->m_Engine->GetGameDir( szGameDir, 2048 );

		/* Get to the /bin/server_i486.so dir */
		char szBinDir[2048];
		V_snprintf(szBinDir, 2048, "%s/bin/server_i486.so", szGameDir);

		/* Open a handle to server_i486.so */
		DevMsg("[SPE]: Path to server_i486.so is %s\n", szBinDir);
		void* handle = dlopen(szBinDir, RTLD_NOW | RTLD_GLOBAL);

		/* Make sure it's valid */
		if( !handle )
		{
			DevMsg("[SPE]: Couldn't open a handle to server_i486.so");
			return NULL;
		}

		/* Otherwise, parse out the symbol */
		const char* nix_symbol;

		if( !PyArg_ParseTuple( args, "s", &nix_symbol ) )
		{
			SPE_PY_EXCEPTION("[SPE]: spe_FindSymbol: Couldn't parse the symbol!\n");
		}

		/* Find the address of the symbol */
		void* sym_addr = dlsym( handle, nix_symbol );

		/* Print it out */
		DevMsg("[SPE]: Symbol address: %i\n", sym_addr);

		/* Close the handle */
		dlclose( handle );

		/* Return it as a CObject */
		return PyCObject_FromVoidPtr( sym_addr, NULL );

	#endif
}

//=============================================================================
// >> Rips a pointer out from an offset and another pointer
//=============================================================================
PyObject* spe_RipPointer( PyObject* self, PyObject* args )
{
	PyObject* func_ptr;
	void* addr = NULL;
	void** new_this = NULL;
	int offset = 0;

	if( !PyArg_ParseTuple(args, "Oi", &func_ptr, &offset) )
	{
		SPE_PY_EXCEPTION("[SPE]: Error parsing tuple in spe_RipPointer!\n");
	}

	addr = PyCObject_AsVoidPtr( func_ptr );

	if( addr )
	{
		new_this = *reinterpret_cast<void ***>((unsigned char*)addr + offset);
		return Py_BuildValue("O", PyCObject_FromVoidPtr( new_this, NULL ));
	}	

	SPE_PY_EXCEPTION( "[SPE]: Could not rip out the pointer!\n" )
}

//=============================================================================
// >> Calls a function through the use of dyncall
//=============================================================================
PyObject* spe_CallFunction( PyObject* self, PyObject* args )
{
	return CallFunction(args);
}

//=============================================================================
// >> Returns a CObject containing the address of a virtual function.
//=============================================================================
PyObject* spe_GetVfnAddress( PyObject* self, PyObject* args )
{
	int index;
	PyObject* cobj;
	void* instance;

	if(!PyArg_ParseTuple(args, "Oi", &cobj, &index))
	{
		SPE_PY_EXCEPTION("[SPE]: You have passed in an invalid instance or index.\n");
	}

	//Get instance
	instance = PyCObject_AsVoidPtr( cobj );

	//Get function address...
	void **vtable = *(void ***)instance;
	void *func = vtable[index];

	//Build CObject
	if( !func )
	{
		SPE_PY_EXCEPTION("[SPE]: Something went terribly wrong. Could not find the function in the vtable!\n");
	}

	return PyCObject_FromVoidPtr( func, NULL );
}

//=============================================================================
// >> Registers a script block for a pre-event hook
//=============================================================================
PyObject* spe_PreHookEvent( PyObject* self, PyObject* args )
{
	char* szEventName;
	PyObject* function;

	if( !PyArg_ParseTuple(args, "sO", &szEventName, &function) )
	{
		SPE_PY_EXCEPTION("[SPE]: Could not parse out objects for spe_preHookEvent!\n");
	}

	if( !PyCallable_Check( function ) )
	{
		SPE_PY_EXCEPTION("[SPE]: Can't call function!\n");
	}

	//Hook the event
	gpHookMan->AddPreHook( szEventName, function );

	return Py_BuildValue("");
}
//=============================================================================
// >> Unregisters a script block for a pre-event hook.
//=============================================================================
PyObject* spe_UnHookEvent( PyObject* self, PyObject* args )
{
	char* szEventName;
	PyObject* function;

	if( !PyArg_ParseTuple(args, "sO", &szEventName, &function) )
	{
		SPE_PY_EXCEPTION("[SPE]: Could not parse out objects for spe_UnHookPreEvent!\n");
	}

	//Unhook the event.
	gpHookMan->RemovePreHook( szEventName, function );

	return Py_BuildValue("");
}

