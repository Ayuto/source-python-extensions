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

#include "esx_hook_manager.h"
#include "esx_event_parser.h"

//Declare the sourcehook prototype!
SH_DECL_HOOK2(IGameEventManager2, FireEvent, SH_NOATTRIB, 0, bool, IGameEvent *, bool);

//==================================================================================
// Constructor
//==================================================================================
CSPEHookManager::CSPEHookManager(IGameEventManager2 *pManager)
{
	//Add the event hook
	SH_ADD_HOOK_MEMFUNC(IGameEventManager2, FireEvent, pManager, this, &CSPEHookManager::EventFire_Pre, false);

	m_Manager = pManager;
}

//==================================================================================
// Destructor
//==================================================================================
CSPEHookManager::~CSPEHookManager()
{
	//Remove the hook
	SH_REMOVE_HOOK_MEMFUNC(IGameEventManager2, FireEvent, m_Manager, this, &CSPEHookManager::EventFire_Pre, false);
}

//==================================================================================
// Registers a python function with it's prehook.
//==================================================================================
void CSPEHookManager::addPreHook(char *szEventName, PyObject *pyFunc)
{
	//Loop through our event list, see if there already is an event with a registered hook.
	PY_EVENT_HOOK *p = NULL;

	DevMsg("[SPE]: Adding a hook for %s.\n", szEventName);

	//Do we have a PY_EVENT_HOOK struct for this event?
	for(int i = 0; i < m_EventHooks.Count(); i++)
	{
		PY_EVENT_HOOK* temp = m_EventHooks.Element(i);
		
		if(temp)
		{
			if( strcmp(szEventName, temp->szEventName) == 0 )
			{
				DevMsg("Event is %s.\n", temp->szEventName);
				p = temp;
				break;
			}
		}
	}

	//If p is still NULL, we need to make the hook and add it.
	if(!p)
	{
		DevMsg("Adding a py_event_hook.\n");
		//Create a hook struct
		p = new PY_EVENT_HOOK();

		//Set the event identifier
		p->szEventName = szEventName;

		//Add the python function to the list
		p->funcList.AddToTail(pyFunc);

		//Add the struct to the list
		m_EventHooks.AddToTail(p);
	}

	//Otherwise, just add the function
	else
		p->funcList.AddToTail(pyFunc);

	//Make sure we are listening to this event
	if( !m_Manager->FindListener(this, szEventName) )
	{
		//Manually add a listener for it.
		if( !m_Manager->AddListener(this, szEventName, true) )
		{
			DevMsg("[SPE]: Could not add a listener for event %s!\n", szEventName);
		}
	}
}

//==================================================================================
// Removes a python function from it's hook
//==================================================================================
void CSPEHookManager::removePreHook(char *szEventName, PyObject *pyFunc)
{
	//Loop through existing python hooks
	for(int i = 0; i < m_EventHooks.Count(); i++)
	{
		PY_EVENT_HOOK* p = m_EventHooks.Element(i);

		if( p )
		{
			//Remove it pyFunc is in the list.
			p->funcList.FindAndRemove( pyFunc );
			DevMsg("Size of funcList is now %i\n", p->funcList.Count());
		}
	}
}

//==================================================================================
// Called on each event fire
//==================================================================================
bool CSPEHookManager::EventFire_Pre(IGameEvent *pEvent, bool bDontBroadcast)
{
	if( !pEvent )
		RETURN_META_VALUE(MRES_HANDLED, false);

	//Check to see if we have any hooks registered for this event.
	const char* name = pEvent->GetName();
	DevMsg("[SPE]: Event name %s\n", name);

	PY_EVENT_HOOK* p = NULL;

	DevMsg("[SPE]: Sizeof m_Eventhooks is %i\n", m_EventHooks.Count());

	//Get the PY_EVENT_HOOK structure
	for(int i = 0; i < m_EventHooks.Count(); i++)
	{
		PY_EVENT_HOOK* temp = m_EventHooks.Element(i);

		if(temp)
		{
			DevMsg("[SPE]: Found a python hook struct!\n");
			if(strcmp(name, temp->szEventName) == 0)
			{
				DevMsg("Found a struct!\n");
				p = temp;
				break;
			}
		}
	}

	if( !p )
	{
		DevMsg("[SPE]: Null pointer.\n");
		RETURN_META_VALUE(MRES_HANDLED, false);
	}

	//Now, loop through each python function and call it.
	for(int i = 0; i < p->funcList.Count(); i++)
	{
		PyObject* pyFunc = p->funcList.Element(i);

		DevMsg("[SPE]: Looping!\n");
		if(pyFunc)
		{
			DevMsg("[SPE]: Calling the function!\n");
			
			PyObject* pDict = g_pParser->getEventVariables( pEvent );
			
			PyEval_CallFunction( pyFunc, "(O)", pDict );
		}
	}

	DevMsg("[SPE]: Executed blocks for event %s!\n", name);

	RETURN_META_VALUE(MRES_HANDLED, true);
}

//==================================================================================
// IGameEventListener2 override
//==================================================================================
void CSPEHookManager::FireGameEvent(IGameEvent *pEvent)
{
	//Do nothing.
}