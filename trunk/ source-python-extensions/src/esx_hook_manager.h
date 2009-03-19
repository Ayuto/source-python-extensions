/**
* =============================================================================
* Eventscripts Extensions
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

#ifndef ESX_HOOK_MANAGER_H
#define ESX_HOOK_MANAGER_H

/* Includes */
#include <Python.h>
#include <utlvector.h>
#include <igameevents.h>
#include "serverplugin_base.h"
 
//==================================================================================
// >> Pools the event_name and python function pointer into a single spot.
//==================================================================================
struct PY_EVENT_HOOK
{
	char* szEventName;
	CUtlVector<PyObject*> funcList; // All of these will be called before the event is fired.
};

//==================================================================================
// >> Manages python/c++ hooks
//==================================================================================
class CSPEHookManager : public IGameEventListener2
{
	public:
		CSPEHookManager( IGameEventManager2* pManager );
		~CSPEHookManager();
		void addPreHook( char* szEventName, PyObject* pyFunc );
		void removePreHook( char* szEventName, PyObject* pyFunc );
		bool EventFire_Pre(IGameEvent *pEvent, bool bDontBroadcast);
		void FireGameEvent(IGameEvent *pEvent);

	private:
		IGameEventManager2* m_Manager;
		CUtlVector<PY_EVENT_HOOK*> m_EventHooks;
};

#endif