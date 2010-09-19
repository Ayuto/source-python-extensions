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
#include "spe_globals.h"
#include "spe_python.h"
#include "spe_dyncall.h"

//=================================================================================
// Sets the calling convention of the VM
//=================================================================================
DECLARE_PYCMD( setCallingConvention, "Sets the calling convention of the virtual machine." )
{
	if( !vm )
	{
		DevMsg("[SPE]: spe_setCallingConvention: The virtual machine pointer is invalid.\n");
		return Py_BuildValue("i", -1);
	}

	const char* conv;
	if( !PyArg_ParseTuple(args, "s", &conv) )
	{
		DevMsg("[SPE]: spe_setCallingConvention: Couldn't parse the arguments.\n");
		return Py_BuildValue("i", -1);
	}

	// Reset the current calling convention.
	dcReset(vm);

	// Valid calling conventions
	if( strcmp(conv, "thiscall") == 0 )
	{
		#ifdef _WIN32
			dcMode(vm, DC_CALL_C_X86_WIN32_THIS_MS);
		#else
			dcMode(vm, DC_CALL_C_X86_WIN32_THIS_GNU);
		#endif
	}

	else if( strcmp(conv, "fastcall") == 0 )
	{
		#ifdef _WIN32
			dcMode(vm, DC_CALL_C_X86_WIN32_FAST_MS);
		#else
			dcMode(vm, DC_CALL_C_X86_WIN32_THIS_GNU);
		#endif
	}

	else if( strcmp(conv, "cdecl") == 0 )
	{
		dcMode(vm, DC_CALL_C_DEFAULT);
	}

	else
	{
		DevMsg("[SPE]: spe_setCallingConvention: Unknown calling convention passed in.\n");
		return Py_BuildValue("i", -1);
	}

	return Py_BuildValue("i", 0);
}

//=================================================================================
// Calls the sigscanned function.
//=================================================================================
DECLARE_PYCMD( callFunction, "Calls the sigscanned function." )
{
	PyObject* func_args = NULL;
	const char* signature = NULL;
	const char* ptr;
	int size;
	int pos = 0;
	char ch;
	void* function_pointer = NULL;

	if( !PyArg_ParseTuple( args, "isO", &function_pointer, &signature, &func_args ) )
	{
		DevMsg("[SPE] Error parsing out function args, signature, and pointer!\n");
		return Py_BuildValue("");
	}

	/* Make sure it's valid */
	if( !function_pointer )
	{
		DevMsg("[SPE] The function pointer is NULL!\n");
		return Py_BuildValue("");
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
					return Py_BuildValue("");
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
					return Py_BuildValue("");
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
				ptr = (DCpointer) ( (DCint) PyLong_AsLongLong(arg) );
				dcArgPointer( vm, ptr );
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
				return Py_BuildValue("");
			} break;
		}

		++pos; ++ptr;

	} //End while

	if (pos != size)
	{
		DevMsg("[SPE] pos != size!\n");
		return Py_BuildValue("");
	}

	if (ch == '\0')
	{
		DevMsg("[SPE] ch == null terminated\n");
		return Py_BuildValue("");
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
				return Py_BuildValue("");

			/* Assign it to p otherwise */
			p = Py_BuildValue("i", (int)ptr);

			break;
		}

		default:
		{
			DevMsg("[SPE] Invalid p = type signature.\n" ); 
			return Py_BuildValue("");
		}
	}

	return p;	
}
