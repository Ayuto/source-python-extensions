//=============================================================================//
// Copyright: Deniz Sezen
//		Defines this modules python methods
//=============================================================================//

#ifndef ESX_PY_METHODS_H
#define ESX_PY_METHODS_H

#ifdef _WIN32
#pragma once
#endif

/* Includes */
#ifdef _WIN32
#include <Python.h>
#else
#include <Python25/Include/Python.h>
#endif

/* Methods */
PyObject* esx_GetPlayerPtr( PyObject* self, PyObject* args );
PyObject* esx_FindFuncPtr( PyObject* self, PyObject* args );
PyObject* esx_CallFunction( PyObject* self, PyObject* args );
PyObject* esx_SetCallingConvention( PyObject* self, PyObject* args );
PyObject* esx_RipPointer( PyObject* self, PyObject* args );

/* Method table */
static PyMethodDef esxMethods[] = {
	{ "getPlayer", esx_GetPlayerPtr, METH_VARARGS, "Returns a pointer to a player by userid." },
	{ "findFunction", esx_FindFuncPtr, METH_VARARGS, "Returns a pointer to a function by signature." },
	{ "callFunction", esx_CallFunction, METH_VARARGS, "Calls a function through the use of a CObject." },
	{ "setCallingConvention", esx_SetCallingConvention, METH_VARARGS, "Sets the CallVM calling convention." },
	{ NULL, NULL, 0, NULL }
};

#endif
