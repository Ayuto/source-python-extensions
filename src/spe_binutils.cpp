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
        Warning("[SPE]: Could not parse the tuple in spe_findFunction.\n");
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
        Warning("[SPE]: spe_findSymbol: Couldn't parse function arguments.\n");
        return Py_BuildValue("");
    }

    // Find the address of the symbol
    void *addr = dlsym( server_handle, symbol );

    // Validate it
    if( !addr )
    {
        Warning("[SPE]: Could not find symbol %s!\n", symbol);
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











