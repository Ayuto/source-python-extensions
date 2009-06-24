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
#include "spe_python.h"
#include "spe_globals.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif


//=================================================================================
// Definitions
//=================================================================================
#define METHOD_TABLE_SIZE 100
#define MODULE_NAME	"spe_C"

//=================================================================================
// Global variables
//=================================================================================
PyMethodDef g_SPEMethods[METHOD_TABLE_SIZE]; // Holds all of our python methods.

//=================================================================================
// Initializes python for us
//=================================================================================
bool EnablePython( void )
{
	if( !engine )
	{
		Error("[SPE]: The engine pointer was not valid.\n");
		return false;
	}

#ifdef _WIN32
	char szGameDir[MAX_STRING_LEN];
	char szPythonDLL[MAX_STRING_LEN];
	char szMSVCRTDLL[MAX_STRING_LEN];

	// Get the game dir.
	engine->GetGameDir(szGameDir, MAX_STRING_LEN);

	// Path to python25.dll and msvcrt.dll
	V_snprintf(szMSVCRTDLL, MAX_STRING_LEN, "%s\\addons\\eventscripts\\_engines\\python\\Lib\\plat-win\\msvcr71.dll", szGameDir);
	V_snprintf(szPythonDLL, MAX_STRING_LEN, "%s\\addons\\eventscripts\\_engines\\python\\Lib\\plat-win\\python25.dll", szGameDir);

	// Load both modules.
	if( !LoadLibrary(szMSVCRTDLL) || !LoadLibrary(szPythonDLL) )
	{
		Error("[SPE]: Could not load one or more required dependencies to initialize python!\n");
		return false;
	}
#else
	if( !dlopen("libpython2.5.so.1.0", RTLD_NOW | RTLD_GLOBAL ) )
	{
		Error("[SPE]: Error. Unable to open libpython2.5.so.1.0!\n");
		return false;
	}
#endif

	// Initialize python
	Py_Initialize();

	// Setup the module
	Py_InitModule(MODULE_NAME, g_SPEMethods);

	return true;
}
