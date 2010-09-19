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
#include "spe_hook_manager.h"
#include "spe_event_parser.h"
#include "spe_python.h"
#include <igameevents.h>

//=================================================================================
// Globals
//=================================================================================
CHookManager* g_pHookManager;

//=================================================================================
// Declare the sourcehook prototype!
//=================================================================================
SH_DECL_HOOK2(IGameEventManager2, FireEvent, SH_NOATTRIB, 0, bool, IGameEvent *, bool);

//=================================================================================
// Constructor
//=================================================================================
CHookManager::CHookManager( IGameEventManager2* pEvMan )
{
	// -----------------------------------
	// Sanity check.
	// -----------------------------------
	if( !pEvMan ) {
		Msg("[SPE]: Warning! pEvMan == NULL! Prehooking not available!\n");
		return;
	}

	// -----------------------------------
	// Set the event manager.
	// -----------------------------------
	m_pManager = pEvMan;

	// -----------------------------------
	// Add the eventmanager hook.
	// -----------------------------------
	SH_ADD_HOOK_MEMFUNC(IGameEventManager2, FireEvent, m_pManager, 
		this, &CHookManager::EventFire_Pre, false);
}

//=================================================================================
// Destructor
//=================================================================================
CHookManager::~CHookManager( void )
{
	// -----------------------------------
	// Remove the hook.
	// -----------------------------------
	SH_REMOVE_HOOK_MEMFUNC(IGameEventManager2, FireEvent, m_pManager, 
		this, &CHookManager::EventFire_Pre, false);

	// -----------------------------------
	// Free up all the memory in the
	// map.
	// -----------------------------------
	m_Hooks.Purge();

	// -----------------------------------
	// Don't listen to events.
	// -----------------------------------
	m_pManager->RemoveListener( this );
}

//=================================================================================
// Adds a prehook to the list.
//=================================================================================
void CHookManager::AddHook( const char* szEventName, PyObject* pyFunc )
{
	// -----------------------------------
	// Sanity checking.
	// -----------------------------------
	if( !szEventName || !pyFunc )
	{
		DevMsg(1, "[SPE]: Warning! CHookManager::AddHook recieved NULL argument!\n");
		return;
	}

	// -----------------------------------
	// Do we already have an EventHook_t
	// for this event?
	// -----------------------------------
	EventHook_t* pHook = NULL;
	FOR_EACH_VEC( m_Hooks, i )
	{
		// -----------------------------------
		// Compare names
		// -----------------------------------
		if( strcmp(m_Hooks[i]->szEventName, szEventName) == 0 )
		{
			// -----------------------------------
			// Yay found it!
			// -----------------------------------
			pHook = m_Hooks[i];

			// -----------------------------------
			// Done.
			// -----------------------------------
			break;
		}
	}

	// -----------------------------------
	// Did we find it?
	// -----------------------------------
	if( !pHook ) 
	{
		// -----------------------------------
		// Create one.
		// -----------------------------------
		pHook = new EventHook_t;

		pHook->szEventName = strdup( szEventName );
		pHook->pCallbacks = new CUtlVector<PyObject *>();

		// -----------------------------------
		// Add it to the hooks list.
		// -----------------------------------
		m_Hooks.AddToTail( pHook );
	}

	// -----------------------------------
	// Add the python function to the
	// list.
	// -----------------------------------
	pHook->pCallbacks->AddToTail( pyFunc );
}

//=================================================================================
// Removes a prehook from the list.
//=================================================================================
void CHookManager::RemoveHook( const char* szEventName, PyObject* pyFunc )
{
	// -----------------------------------
	// Sanity checking.
	// -----------------------------------
	if( !szEventName || !pyFunc )
	{
		DevMsg(1, "[SPE]: Warning! CHookManager::AddHook recieved NULL argument!\n");
		return;
	}

	// -----------------------------------
	// Do we have a hook structure for
	// this event?
	// -----------------------------------
	EventHook_t* pHook = NULL;
	FOR_EACH_VEC( m_Hooks, i )
	{
		if( strcmp(m_Hooks[i]->szEventName, szEventName) == 0 )
		{
			// -----------------------------------
			// Yay found it!
			// -----------------------------------
			pHook = m_Hooks[i];

			// -----------------------------------
			// Done.
			// -----------------------------------
			break;
		}
	}

	// -----------------------------------
	// Did we find a hook structure?
	// -----------------------------------
	if( !pHook ) 
	{
		// -----------------------------------
		// Can't continue.
		// -----------------------------------
		return;
	}

	// -----------------------------------
	// Get the callback list.
	// -----------------------------------
	CUtlVector<PyObject *>* pCallbacks =
		pHook->pCallbacks;

	// -----------------------------------
	// Find the python function in the
	// vector.
	// -----------------------------------
	int idx = pCallbacks->Find( pyFunc );

	// -----------------------------------
	// Make sure it's valid.
	// -----------------------------------
	if( idx != pCallbacks->InvalidIndex() )
	{
		// -----------------------------------
		// Remove it.
		// -----------------------------------
		pCallbacks->Remove( idx );
	}

	// -----------------------------------
	// Are we at 0 hooks for this event?
	// -----------------------------------
	if( pCallbacks->Count() == 0 )
	{
		DevMsg(1, "[SPE]: %s has zero callbacks. Deleting from list..\n", szEventName);

		// -----------------------------------
		// Get rid of the event hook.
		// -----------------------------------
		pCallbacks->PurgeAndDeleteElements();
		m_Hooks.FindAndRemove( pHook );
		
		delete pCallbacks;
		delete pHook;
	}
}

//=================================================================================
// Called when an event is fired.
//=================================================================================
bool CHookManager::EventFire_Pre( IGameEvent *pEvent, bool bDontBroadcast )
{
	// -----------------------------------
	// Sanity checking.
	// -----------------------------------
	if( !pEvent || !pEvent->GetName() )
	{
		RETURN_META_VALUE( MRES_HANDLED, false );
	}

	// -----------------------------------
	// Do we have a hook registered for this
	// event?
	// -----------------------------------
	const char* szEventName = pEvent->GetName();
	DevMsg(2, "[SPE]: Event name is %s\n", szEventName);

	// -----------------------------------
	// Look for the vector that pairs
	// with the given event name.
	// -----------------------------------
	CUtlVector<PyObject*>* pCallbackList = NULL;
	
	// -----------------------------------
	// Do we have a hook structure for
	// this event?
	// -----------------------------------
	FOR_EACH_VEC( m_Hooks, i )
	{
		if( strcmp(m_Hooks[i]->szEventName, szEventName) == 0 )
		{
			// -----------------------------------
			// Yay found it!
			// -----------------------------------
			pCallbackList = m_Hooks[i]->pCallbacks;

			// -----------------------------------
			// Done.
			// -----------------------------------
			break;
		}
	}

	// -----------------------------------
	// If it's not valid, we can't
	// continue.
	// -----------------------------------
	if( !pCallbackList )
	{
		DevMsg(2, "[SPE]: Could not find callback list for %s!\n", szEventName);
		RETURN_META_VALUE( MRES_HANDLED, false );
	}

	// -----------------------------------
	// Get the python representation for
	// the event data.
	// -----------------------------------
	PyObject* pEventDict = g_pParser->GetEventVariables( pEvent );

	// -----------------------------------
	// Loop through all the callbacks and
	// execute them.
	// -----------------------------------
	for( int i = 0; i < pCallbackList->Count(); i++ )
	{
		// -----------------------------------
		// Get the python function.
		// -----------------------------------
		PyObject* pCallback = pCallbackList->Element(i);

		// -----------------------------------
		// Execute it.
		// -----------------------------------
		DevMsg(2, "[SPE]: Calling callback for %s!\n", szEventName);
		PyObject* retVal = PyEval_CallFunction( pCallback, "(O)", pEventDict );

		// -----------------------------------
		// Decrement the reference count.
		// -----------------------------------
		Py_XDECREF( retVal );
	}

	Py_DECREF( pEventDict );

	// -----------------------------------
	// Done.
	// -----------------------------------
	RETURN_META_VALUE( MRES_HANDLED, true );
}

//=================================================================================
// IGameEventListener2 override.
//=================================================================================
void CHookManager::FireGameEvent( IGameEvent *pEvent )
{
	// -----------------------------------
	// Do nothing.
	// -----------------------------------
}

//==================================================================================
// Registers a prehook.
//==================================================================================
DECLARE_PYCMD( registerPreHook, "Registers an event prehook." )
{
	char	*szEventName;
	PyObject *callback;

	// -----------------------------------
	// Parse arguments from python.
	// -----------------------------------
	if( !PyArg_ParseTuple(args, "sO", &szEventName, &callback) )
	{
		DevMsg(1, "[SPE]: spe_RegisterPreHook: Could not parse function arguments.\n");
		return NULL;
	}

	// -----------------------------------
	// Add the hook.
	// -----------------------------------
	g_pHookManager->AddHook(szEventName, callback);

	// -----------------------------------
	// Return.
	// -----------------------------------
	return Py_BuildValue("i", 0);
}

//==================================================================================
// Unregisters a prehook
//==================================================================================
DECLARE_PYCMD( unregisterPreHook, "Unregisters an event prehook." )
{
	const char	*szEventName;
	PyObject	*callback;

	// -----------------------------------
	// Parse arguments from python.
	// -----------------------------------
	if( !PyArg_ParseTuple(args, "sO", &szEventName, &callback) )
	{
		Msg("[SPE]: spe_unregisterPreHook: Could not parse function arguments.\n");
		return NULL;
	}

	// -----------------------------------
	// Remove the hook.
	// -----------------------------------
	g_pHookManager->RemoveHook(szEventName, callback);

	// -----------------------------------
	// Done.
	// -----------------------------------
	return Py_BuildValue("i", 0);
}

