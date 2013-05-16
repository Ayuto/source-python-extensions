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
#include "spe_globals.h"
#include "spe_python.h"
#include "spe_dyncall.h"
#include "spe_exceptions.h"

//=================================================================================
// Sets the calling convention of the VM
//=================================================================================
DECLARE_PYCMD( setCallingConvention, "Sets the calling convention of the virtual machine." )
{
    const char* conv;
    if( !PyArg_ParseTuple(args, "s", &conv) )
    {
        DevMsg("[SPE] spe_setCallingConvention: Couldn't parse the arguments.\n");
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
        DevMsg("[SPE] spe_setCallingConvention: Unknown calling convention passed in.\n");
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
        PyObject* arg = PyTuple_GetItem( func_args, pos );

        /* Do some comparisons */
        switch( ch )
        {
            case DC_SIGCHAR_BOOL: dcArgBool( vm, PyInt_AsLong(arg) ); break;
            case DC_SIGCHAR_CHAR:
            {
                if (PyString_GET_SIZE(arg) != 1)
                {
                    DevMsg("[SPE] String mismatch. Expected a string!");
                    return Py_BuildValue("");
                }
                dcArgChar( vm, *PyString_AsString(arg) );
            } break;

            case DC_SIGCHAR_SHORT:
            {
                long v = PyInt_AS_LONG(arg);
                if ( (v < SHRT_MIN) || (v > SHRT_MAX) )
                {
                    DevMsg( "[SPE] CallFunction: value out of range at argument %d - expecting a short value\n" );
                    return Py_BuildValue("");
                }
                dcArgShort( vm, (DCshort) v);

            } break;

            case DC_SIGCHAR_INT:     dcArgInt( vm, (DCint) PyInt_AsLong( arg ) );            break;
            case DC_SIGCHAR_UINT:    dcArgLong( vm, (DCuint) PyLong_AsLong( arg ));          break;
            case DC_SIGCHAR_FLOAT:   dcArgFloat( vm, (float) PyFloat_AsDouble( arg ) );      break;
            case DC_SIGCHAR_POINTER: dcArgPointer( vm, (DCpointer) PyInt_AsLong(arg) );      break;
            case 'S':                dcArgPointer( vm, (DCpointer) PyString_AsString(arg) ); break;
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

    PyObject* p = NULL;
    START_CATCH_EXC
    switch(ch)
    {
        case DC_SIGCHAR_VOID:     p = Py_BuildValue(""); dcCallVoid( vm, function_pointer ); break;
        case DC_SIGCHAR_BOOL:     p = Py_BuildValue("i", dcCallBool( vm, function_pointer ) ); break;
        case DC_SIGCHAR_INT:      p = Py_BuildValue("i", dcCallInt( vm, function_pointer ) ); break;
        case DC_SIGCHAR_LONGLONG: p = Py_BuildValue("L", (unsigned long long) dcCallLongLong( vm, function_pointer ) ); break;
        case DC_SIGCHAR_FLOAT:    p = Py_BuildValue("f", dcCallFloat( vm, function_pointer ) ); break;
        case DC_SIGCHAR_DOUBLE:   p = Py_BuildValue("d", dcCallDouble( vm, function_pointer ) ); break;
        case 's':                 p = Py_BuildValue("s", dcCallPointer( vm, function_pointer ) ); break;
        case DC_SIGCHAR_POINTER:  p = Py_BuildValue("i", (int) dcCallPointer(vm, function_pointer)); break;
        default:
        {
            DevMsg("[SPE] Invalid return type: %c\n", ch);
            return Py_BuildValue("");
        }
    }
    END_CATCH_EXC
    return p ? p : Py_BuildValue("");
}
