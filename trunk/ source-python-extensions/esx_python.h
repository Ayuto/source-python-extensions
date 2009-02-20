//=============================================================================//
// Copyright: Deniz Sezen
//		Derived from the work of VALVe Software
//=============================================================================//

#ifndef __ESX_PYTHON_H__
#define __ESX_PYTHON_H__

/* Includes */
#ifdef _WIN32
#include <windows.h>
#include <Python.h>
#else
#include <dlfcn.h>
#include <Python25/Include/Python.h>
#endif 
#include "esx_dyncall_py.h"
#include "esx_py_methods.h"

bool initializePython( char *pGameDir )
{
	char pAddonDir[1024];

#ifdef _WIN32
	V_snprintf( pAddonDir, 1024, "%s\\addons\\eventscripts", pGameDir );

	/* Load the libraries */
	HMODULE pPythonDLL = NULL;
	HMODULE pMSVCRDLL = NULL;

	char pDLL1[1024];
	char pDLL2[1024];

	V_snprintf( pDLL1, 1024, "%s\\_engines\\python\\Lib\\plat-win\\msvcr71.dll", pAddonDir );
	V_snprintf( pDLL2, 1024, "%s\\_engines\\python\\Lib\\plat-win\\python25.dll", pAddonDir );

	pMSVCRDLL = LoadLibrary( pDLL1 );
	pPythonDLL = LoadLibrary( pDLL2 );

	if( !pPythonDLL || !pMSVCRDLL )
	{
		Msg("[ESX] An error occured. Could not find msvcr71.dll or python25.DLL.\n");
		return false;
	}

#else
	/*V_snprintf( pAddonDir, 1024, "%s/addons/eventscripts", pGameDir );

	char pPythonSO[1024];
	V_snprintf( pPythonSO, 1024, "%s/_engines/python/Lib/plat-linux2/libpython2.5.so.1.0", pAddonDir );

	if( !dlopen( pPythonSO, RTLD_GLOBAL ) )
	{
		Msg("[ESX] An error has occured. Could not find libpython2.5.so.1.0\n");
		return false;
	}*/

	if( !dlopen("libpython2.5.so.1.0", RTLD_NOW | RTLD_GLOBAL) )
	{
		Log("[ESX] An error has occured. Could not load libpython2.5.so.1.0\n!");
		return false;
	}
#endif

	Py_Initialize();

	/* Initialize the module */
	Py_InitModule( "esx_C", esxMethods );
	
	return true;
}

#endif
