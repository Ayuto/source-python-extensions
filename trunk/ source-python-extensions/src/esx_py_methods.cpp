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
#include "esx_py_methods.h"
#include "esx_signature_manager.h"
#include "esx_dyncall_py.h"
#include "esx_globals.h"
#include "esx_hook_manager.h"
#ifdef __linux__
#include <dlfcn.h>
#endif

//Does error handling for us
#define SPE_PY_EXCEPTION( reason ) \
	DevMsg( reason ); \
	PyErr_SetString(NULL, reason ); \
	return NULL;

/* A new call VM */
DCCallVM* vm = dcNewCallVM( 4096 );

//=============================================================================
// >> Returns a CObject to a userid.
//=============================================================================
PyObject* esx_GetPlayerPtr( PyObject* self, PyObject* args )
{
	int userid = 0;

	if( !PyArg_ParseTuple( args, "i", &userid ) )
	{
		SPE_PY_EXCEPTION( "[SPE] Could not parse tuple in esx_GetPlayerPtr!\n" );
	}

	void* ent = gPlayerManager->GetPlayerByUserID(userid)->GetUnknown()->GetBaseEntity();

	if( !ent )
	{
		SPE_PY_EXCEPTION( "[SPE] The entity was null in esx_GetPlayerPtr!\n" );
	}

	return PyCObject_FromVoidPtr( ent, NULL );
}

//=============================================================================
// >> Returns a CObject to a function, using a signature
//=============================================================================
PyObject* esx_FindFuncPtr( PyObject* self, PyObject* args )
{
	PyObject* p;
	int length = 0;

	if( !PyArg_ParseTuple( args, "Si", &p, &length ) )
	{
		SPE_PY_EXCEPTION( "[SPE] Could not parse tuple in esx_FindfuncPtr!\n" );
	}

	void* addr = gSigger->findFunction( PyString_AsString(p), length );
	DevMsg("[SPE] Function ptr: %i\n", addr);
	
	if( !addr )
	{
		SPE_PY_EXCEPTION( "[SPE] Error! Could not find signature!" )
	}
	
	return PyCObject_FromVoidPtr( addr , NULL );
}

//=============================================================================
// >> Sets the calling convention of the virtual machine
//=============================================================================
PyObject* esx_SetCallingConvention( PyObject* self, PyObject* args )
{
	const char* conv;

	if( !PyArg_ParseTuple( args, "s", &conv ) )
	{
		SPE_PY_EXCEPTION( "[SPE] Could not parse tuple in esx_SetCallingConvention!\n" )
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
PyObject* esx_FindSymbol( PyObject* self, PyObject* args )
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
			SPE_PY_EXCEPTION("[SPE]: esx_FindSymbol: Couldn't parse the symbol!\n");
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
PyObject* esx_RipPointer( PyObject* self, PyObject* args )
{
	PyObject* func_ptr;
	void* addr = NULL;
	void** new_this = NULL;
	int offset = 0;

	if( !PyArg_ParseTuple(args, "Oi", &func_ptr, &offset) )
	{
		SPE_PY_EXCEPTION("[SPE]: Error parsing tuple in esx_RipPointer!\n");
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
PyObject* esx_CallFunction( PyObject* self, PyObject* args )
{
	PyObject* func_ptr = NULL;
	PyObject* func_args = NULL;
	const char* signature = NULL;
	const char* ptr;
	int size;
	int pos = 0;
	char ch;
	void* function_pointer = NULL;

	if( !PyArg_ParseTuple( args, "OsO", &func_ptr, &signature, &func_args ) )
	{
		SPE_PY_EXCEPTION("[SPE] Error parsing out function args, signature, and pointer!\n");
	}

	/* Parse out the function pointer */
	function_pointer = PyCObject_AsVoidPtr( func_ptr );

	/* Make sure it's valid */
	if( !function_pointer )
	{
		SPE_PY_EXCEPTION("[SPE] The function pointer is NULL!\n");
	}

	/* Make sure to flush the VM */
	dcReset( vm );

	/* Get the size of the argument list */
	size = PyTuple_Size( func_args );

	ptr = signature;

	/* Start going through the arguments */
	while ( (ch = *ptr) != '\0' && ch != ')' ) 
	{
		/* Create a pyobject to hold the argument */
		PyObject* arg;

		//Increment the index
		//int index = pos + 1;

		arg = PyTuple_GetItem( func_args, pos );

		/* Do some comparisons */
		switch( ch )
		{			
			case DC_SIGCHAR_BOOL:
			{
				DCbool b;
				PyArg_ParseTuple( arg, "i", &b );
				dcArgBool( vm, b );

			} break;

			case DC_SIGCHAR_CHAR:
			{
				DCchar c;
				size_t l;
				char* s;
				l = PyString_GET_SIZE(arg);

				if (l != 1)
				{
					SPE_PY_EXCEPTION("[SPE] String mismatch. Expected a string!");
				}

				s = PyString_AsString(arg);          
				c = (DCchar) s[0];
				dcArgChar( vm, c );

			} break;

			case DC_SIGCHAR_SHORT:
			{
				DCshort s;
				long v;
				v = PyInt_AS_LONG(arg);

				if ( (v < SHRT_MIN) || (v > SHRT_MAX) )
				{
					SPE_PY_EXCEPTION( "[SPE] CallFunction: value out of range at argument %d - expecting a short value\n" );
				}

				s = (DCshort) v;
				dcArgShort( vm, s);

			} break;

			case DC_SIGCHAR_INT:
			{
				int v;
				v = PyInt_AsLong( arg );
				dcArgInt( vm, (DCint) v );
			} break;

			case DC_SIGCHAR_FLOAT:
			{
				DCfloat f;
				f = (float) PyFloat_AsDouble( arg );
				dcArgFloat( vm, f );
			
			} break;

			case DC_SIGCHAR_POINTER:
			{
				DCpointer ptr;
				ptr = PyCObject_AsVoidPtr( arg );
				dcArgPointer( vm, reinterpret_cast<void*>(ptr) );
			
			} break;

			case 'S':
			{
				char* p;
				p = PyString_AsString(arg);
				dcArgPointer( vm, (DCpointer) p );
			
			} break;

			default:
			{
				SPE_PY_EXCEPTION("[SPE] Unknown char signature!\n");
			} break;
		}

		++pos; ++ptr;

	} //End while

	if (pos != size)
	{
		SPE_PY_EXCEPTION("[SPE] pos != size!\n");
	}

	if (ch == '\0')
	{
		SPE_PY_EXCEPTION("[SPE] ch == null terminated\n");
	}

	ch = *++ptr;

	PyObject* p;
	switch(ch) 
	{
	case DC_SIGCHAR_VOID: dcCallVoid( vm, function_pointer ); p = Py_BuildValue(""); break;
	case DC_SIGCHAR_BOOL: p = Py_BuildValue("i", dcCallBool( vm, function_pointer ) ); break;
	case DC_SIGCHAR_INT: p = Py_BuildValue("i", dcCallInt( vm, function_pointer ) ); break;
	case DC_SIGCHAR_LONGLONG: p = Py_BuildValue("L", (unsigned long long) dcCallLongLong( vm, function_pointer ) ); break;
	case DC_SIGCHAR_FLOAT: p = Py_BuildValue("f", dcCallFloat( vm, function_pointer ) ); break; 
	case DC_SIGCHAR_DOUBLE: p = Py_BuildValue("d", dcCallDouble( vm, function_pointer ) ); break;
	case 's': p = Py_BuildValue("s", dcCallPointer( vm, function_pointer ) ); break;
	case DC_SIGCHAR_POINTER:
		{
			/* Call the function */
			void* ptr = dcCallPointer(vm, function_pointer);
		
			/* Is it valid*/
			if( !ptr )
			{
				SPE_PY_EXCEPTION("[SPE]Your function has returned a null pointer.\n");
			}
		
			/* Assign it to p otherwise */
			p = PyCObject_FromVoidPtr(ptr, NULL);

			break;
		}
	
	default:  SPE_PY_EXCEPTION("[SPE] Invalid p = type signature.\n" ); break;
	}

	return p;
}

//=============================================================================
// >> Registers a script block for a pre-event hook
//=============================================================================
PyObject* esx_PreHookEvent( PyObject* self, PyObject* args )
{
	char* szEventName;
	PyObject* function;

	if( !PyArg_ParseTuple(args, "sO", &szEventName, &function) )
	{
		SPE_PY_EXCEPTION("[SPE]: Could not parse out objects for esx_preHookEvent!\n");
	}

	if( !PyCallable_Check( function ) )
	{
		SPE_PY_EXCEPTION("[SPE]: Can't call function!\n");
	}

	//Hook the event
	gGlobals->m_Hooker->addPreHook( szEventName, function );

	return Py_BuildValue("");
}
//=============================================================================
// >> Unregisters a script block for a pre-event hook.
//=============================================================================
PyObject* esx_UnHookEvent( PyObject* self, PyObject* args )
{
	char* szEventName;
	PyObject* function;

	if( !PyArg_ParseTuple(args, "sO", &szEventName, &function) )
	{
		SPE_PY_EXCEPTION("[SPE]: Could not parse out objects for esx_UnHookPreEvent!\n");
	}

	//Unhook the event.
	gGlobals->m_Hooker->removePreHook( szEventName, function );

	return Py_BuildValue("");
}