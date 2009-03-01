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
#ifdef __linux__
#include <dlfcn.h>
#endif

/* A new call VM */
DCCallVM* vm = dcNewCallVM( 4096 );
bool bThisCallActive = false;

//=============================================================================
// >> Returns a CObject to a userid.
//=============================================================================
PyObject* esx_GetPlayerPtr( PyObject* self, PyObject* args )
{
	int userid = 0;

	if( !PyArg_ParseTuple( args, "i", &userid ) )
	{
		DevMsg("[SPE] Could not parse tuple in GetPlayerPtr!\n");
		return NULL;
	}

	void* ent = gPlayerManager->GetPlayerByUserID(userid)->GetUnknown()->GetBaseEntity();
	DevMsg("[SPE] Player pointer: %i\n", ent);
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
		DevMsg("[SPE] Could not parse tuple in FindFuncPtr\n");
		return NULL;
	}

	void* addr = gSigger->findFunction( PyString_AsString(p), length );
	DevMsg("[SPE] Function ptr: %i\n", addr);
	
	if( !addr )
		return NULL;
	
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
		DevMsg("[SPE] Could not parse tuple in SetCallingConvention!\n");
		return Py_BuildValue("");
	}

	dcReset( vm );

	if( strcmp( conv, "thiscall" ) == 0 )
	{
		#ifdef _WIN32
			dcMode( vm, DC_CALL_C_X86_WIN32_THIS_MS );
		#else
			dcMode( vm, DC_CALL_C_X86_WIN32_THIS_GNU );
		#endif
		
		bThisCallActive = true;

		return Py_BuildValue("");
	}

	else if( strcmp( conv, "fastcall" ) == 0 )
	{
		#ifdef _WIN32
			dcMode( vm, DC_CALL_C_X86_WIN32_FAST_MS );
		#else
			dcMode( vm, DC_CALL_C_X86_WIN32_FAST_GNU);
		#endif

		return Py_BuildValue("");
	}

	else if( strcmp( conv, "cdecl" ) == 0 )
	{
		dcMode( vm, DC_CALL_C_DEFAULT );
		return Py_BuildValue("");
	}

	else
	{
		DevMsg("[SPE] Unknown calling convention type!\n");
		return NULL;
	}

	return NULL;
}

//=============================================================================
// >> Finds and returns the address of a function symbol on linux
//=============================================================================
PyObject* esx_FindSymbol( PyObject* self, PyObject* args )
{
	#ifdef _WIN32
		DevMsg("[SPE]: You cannot use this function on windows!\n");
		return Py_BuildValue("");
	#else
		/* Path to the game dir */
		char szGameDir[2048];
		gGlobals->m_Engine->GetGameDir( szGameDir, 2048 );

		/* Get to the /bin/server_i486.so dir */
		char szBinDir[2048];
		V_snprintf(szBinDir, 2048, "%s/bin/server_i486.so", szGameDir);

		/* Open a handle to server_i486.so */
		Msg("[SPE]: Path to server_i486.so is %s\n", szBinDir);
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
			DevMsg("[SPE]: esx_FindSymbol: Couldn't parse the symbol!\n");
			return NULL;
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
	void* cfunc_ptr = NULL;
	void* new_pointer = NULL;
	char* offset;

	if( !PyArg_ParseTuple( args, "Os", &func_ptr, &offset ) )
	{
		Msg("[SPE]: There was an error parsing the tuple!\n");
		return NULL;
	}

	cfunc_ptr = PyCObject_AsVoidPtr( func_ptr );

	Msg("[SPE]: The address is %i.\n", cfunc_ptr);
	Msg("[SPE]: The offset is %i.\n", strtol(offset, NULL, 0));

	/* Rip the pointer */
	if( cfunc_ptr )
	{
		memcpy(&new_pointer, ((char *)cfunc_ptr + strtol(offset, NULL, 0)), sizeof(char *));
		Msg("[SPE]: The new pointer %i.\n", new_pointer);
		return PyCObject_FromVoidPtr( reinterpret_cast<void*>(new_pointer), NULL );
	}

	else
	{
		Msg("[SPE]: The given function pointer was null.\n");
		return NULL;
	}

	return Py_BuildValue("");
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
		DevMsg("[SPE] Error parsing out function args, signature, and pointer!\n");
		return NULL;
	}

	/* Parse out the function pointer */
	function_pointer = PyCObject_AsVoidPtr( func_ptr );

	/* Make sure it's valid */
	if( !function_pointer )
	{
		DevMsg("[SPE] The function pointer is NULL!\n");
		return NULL;
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
					DevMsg("[SPE] String mismatch. Expected a string!");
					return NULL;
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
					DevMsg( "[SPE] CallFunction: value out of range at argument %d - expecting a short value\n" );
					return NULL;
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
				DevMsg("[SPE] Unknown char signature!\n");
				return NULL;
			
			} break;
		}

		++pos; ++ptr;

	} //End while

	if (pos != size)
	{
		DevMsg("[SPE] pos != size!\n");
		return NULL;
	}

	if (ch == '\0')
	{
		DevMsg("[SPE] ch == null terminated\n");
		return NULL;
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
	case DC_SIGCHAR_POINTER: p = Py_BuildValue("O", PyCObject_FromVoidPtr(dcCallPointer( vm, function_pointer ), NULL)); break;
	default:  DevMsg("[SPE] Invalid p = type signature.\n" ); p = NULL; break;
	}

	return p;
}
