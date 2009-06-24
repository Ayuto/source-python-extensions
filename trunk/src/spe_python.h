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

#ifndef SPE_PYTHON_H
#define SPE_PYTHON_H

#ifdef _WIN32
#pragma once
#endif

//=================================================================================
// Includes
//=================================================================================
#include "Python.h"
#include "spe_globals.h"

//=================================================================================
// Python related globals
//=================================================================================
extern PyMethodDef g_SPEMethods[]; // Holds all of our python methods.

//=================================================================================
// Python command class ( Courtesy of Mattie Casper )
//=================================================================================
class PyCommand
{
public:
	PyCommand(char *name, PyCFunction pFunc, unsigned int method, char *desc)
	{
		static int i = 0;
		g_SPEMethods[i].ml_name = name;
		g_SPEMethods[i].ml_meth = pFunc;
		g_SPEMethods[i].ml_flags = method;
		g_SPEMethods[i].ml_doc = desc;
		i++;
		g_SPEMethods[i].ml_name = 0;
		g_SPEMethods[i].ml_meth = 0;
		g_SPEMethods[i].ml_flags = 0;
		g_SPEMethods[i].ml_doc = 0;
	}
};

//=================================================================================
// Useful Macros
//=================================================================================
#define DECLARE_PYCMD( name, docstring ) \
	static PyObject *spe_##name(PyObject *self, PyObject *args); \
	static PyCommand spe_##name##_cmd( #name, spe_##name, METH_VARARGS, docstring ); \
	static PyObject *spe_##name(PyObject *self, PyObject *args)

//=================================================================================
// Functions
//=================================================================================
bool EnablePython( void );

#endif // SPE_PYTHON_H

