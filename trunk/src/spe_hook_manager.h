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
#ifndef _SPE_HOOK_MANAGER
#define _SPE_HOOK_MANAGER

//=================================================================================
// Includes
//=================================================================================
#include <Python.h>
#include <utlvector.h>
#include <igameevents.h>

//=================================================================================
// A single event element.
//=================================================================================
struct EventHook_t
{
	const char* szEventName;
	CUtlVector<PyObject*>* pCallbacks;
};

//=================================================================================
// The hook manager class.
//=================================================================================
class CHookManager : public IGameEventListener2
{
	private:
		// -----------------------------------
		// The game event manager instance.
		// -----------------------------------
		IGameEventManager2* m_pManager;

		// -----------------------------------
		// Pair up the python callbacks with 
		// the event name.
		// -----------------------------------
		CUtlVector<EventHook_t *> m_Hooks;

	public:
		
		// -----------------------------------
		// Constructor / Destructor
		// -----------------------------------
		CHookManager( IGameEventManager2* pEvMan );
		~CHookManager( void );

		// -----------------------------------
		// Callback adding / removal.
		// -----------------------------------
		void AddHook( const char* szEventName, PyObject* pyFunc );
		void RemoveHook( const char* szEventName, PyObject* pyFunc );

		// -----------------------------------
		// Handlers and overrides.
		// -----------------------------------
		bool EventFire_Pre( IGameEvent *pEvent, bool bDontBroadcast );
		void FireGameEvent( IGameEvent *pEvent );
};

//=================================================================================
// The hook manager.
//=================================================================================
extern CHookManager* g_pHookManager; 

#endif // _SPE_HOOK_MANAGER

