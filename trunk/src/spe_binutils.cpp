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
#include "spe_scanner.h"

//=================================================================================
// Creates and returns a pointer to some space
//=================================================================================
DECLARE_PYCMD( alloc, "Allocates space and returns a pointer to it." )
{
	int iBytesToAlloc;
	void *pAlloc = NULL;

	if( !PyArg_ParseTuple(args, "i", &iBytesToAlloc ) )
	{
		DevMsg("[SPE]: spe_alloc: Could not parse arguments.\n");
		return Py_BuildValue("");
	}

	pAlloc = malloc( iBytesToAlloc );

	if( !pAlloc )
	{
		DevMsg("[SPE]: Could not create memory!\n");
		return Py_BuildValue("");
	}

	return Py_BuildValue("i", pAlloc);
}

//=================================================================================
// Deallocates passed in memory
//=================================================================================
DECLARE_PYCMD( dealloc, "Deallocates memory" )
{
	void *addr = NULL;

	if( !PyArg_ParseTuple(args, "i", &addr) )
	{
		DevMsg("[SPE]: spe_alloc: Could not parse arguments.\n");
		return Py_BuildValue("");
	}
	
	if( !addr )
		return Py_BuildValue("");

	free( addr );

	return Py_BuildValue("");
}

//=================================================================================
// Modifies memory at a particular memory location
//=================================================================================
DECLARE_PYCMD( setLocVal, "Sets the contents of a particular memory location" )
{
	char      type = NULL;
	void*     addr = NULL;
	PyObject* val  = NULL;

	if( !PyArg_ParseTuple(args, "ciO", &type, &addr, &val) )
	{
		DevMsg("[SPE]: spe_setLocVal: Can't parse arguments.\n");
		return Py_BuildValue("");
	}

	if(!addr)
		return Py_BuildValue("");

	switch(type)
	{
		case DC_SIGCHAR_BOOL:
		{
			bool b;
			PyArg_ParseTuple( val, "i", &b );
			*(bool *)((char *)addr) = b;
		}

		case DC_SIGCHAR_INT:
		{	
			int v = PyInt_AsLong( val );
			*(int *)((char *)addr) = v;
		} break;

		case DC_SIGCHAR_POINTER:
		{
			DCpointer ptr;
			ptr = (DCpointer) ( (DCint) PyLong_AsLongLong(val) );
			*(DCpointer *)((char *)addr) = ptr;
		} break;

		case DC_SIGCHAR_FLOAT:
		{
			float f;
			f = (float) PyFloat_AsDouble( val );
			*(float *)((char *)addr) = f;
		} break;

		default:
		{
			DevMsg("[SPE]: setLocVal: A valid type indicator was not passed in!\n");
		} break;
	}

	return Py_BuildValue("");
}

//=================================================================================
// Returns what is stored at a particular memory location
//=================================================================================
DECLARE_PYCMD( getLocVal, "Sets the contents of a particular memory location" )
{
	char      type = NULL;
	void*     addr = NULL;

	if( !PyArg_ParseTuple(args, "ci", &type, &addr) )
	{
		DevMsg("[SPE]: spe_setLocVal: Can't parse arguments.\n");
		return Py_BuildValue("");
	}

	if(!addr)
		return Py_BuildValue("");

	switch(type)
	{
		case DC_SIGCHAR_BOOL:
			return Py_BuildValue("b", *(bool *)((char *)addr));

		case DC_SIGCHAR_INT:
			return Py_BuildValue("i", *(int *)((char *)addr));
		
		case DC_SIGCHAR_POINTER:
		{
			void* temp = NULL;
			memcpy(temp, *(DCpointer *)((char *)addr), sizeof(char *));
			return Py_BuildValue("i", temp);
		};

		case DC_SIGCHAR_FLOAT:
			return Py_BuildValue("f", *(float *)((char *)addr));
		
		default:
		{
			DevMsg("[SPE]: setLocVal: A valid type indicator was not passed in!\n");
		} break;
	}

	return Py_BuildValue("");
}


//=================================================================================
// Returns the address of a signature.
//=================================================================================
#ifdef _WIN32
DECLARE_PYCMD( findFunction, "Finds and returns the address of a signature." )
{
    // We need to store the identifier ( hex / symbol ).
    PyObject *pIdent;
    
    // And if on windows, a signature length
    int      iSigLength;
    
    // Parse the tuple
    if( !PyArg_ParseTuple(args, "Oi", &pIdent, &iSigLength) )
    {
        DevMsg("[SPE]: Could not parse the tuple in spe_findFunction.\n");
        return Py_BuildValue("");
    }

    return Py_BuildValue("i", (int)g_SigMngr.ResolveSig(laddr, PyString_AsString(pIdent), iSigLength));
}

#else
//=================================================================================
// Returns the address of a symbol.
//=================================================================================
DECLARE_PYCMD( findSymbol, "Returns the address of a symbol." )
{
    // Parse out symbol
    char* symbol;

    if( !PyArg_ParseTuple(args, "s", &symbol) )
    {
        DevMsg("[SPE]: spe_findSymbol: Couldn't parse function arguments.\n");
        return Py_BuildValue("");
    }

    // Find the address of the symbol
    void *addr = dlsym( laddr, symbol );

    // Validate it
    if( !addr )
    {
        DevMsg("[SPE]: Could not find symbol %s!\n", symbol);
        return Py_BuildValue("");
    }

    // Print it out
	DevMsg("************************************\n");
	DevMsg("[SPE]: Symbol: %s.\n", symbol);
    DevMsg("[SPE]: Symbol address: %d.\n", addr);
	DevMsg("************************************\n");

    // Return it
    return Py_BuildValue("i", (int)addr);
}

#endif // _WIN32

