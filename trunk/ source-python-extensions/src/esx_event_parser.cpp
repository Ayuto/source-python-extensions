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

#include "esx_event_parser.h"
#include <KeyValues.h>
#include "esx_globals.h"

CModEventParser* g_pParser;

//==================================================================================
// >> Constructor
//==================================================================================
CModEventParser::CModEventParser()
{
	//Parse out the events
	parseEvents();
}

//==================================================================================
// >> Parses out the events for each mod
//==================================================================================
void CModEventParser::parseEvents()
{
	KeyValues* pModEventFile = new KeyValues("events");

	//Load the mod event file
	if( !pModEventFile->LoadFromFile(gGlobals->m_Sys, "resource/ModEvents.res") )
	{
		DevMsg("[SPE]: Could not load the mod events file. Event vars are disabled.\n");
		return;
	}

	//Parse through all events
	KeyValues* cur_event = NULL;
	KeyValues* cur_field = NULL;

	//Get the first event and subkey
	cur_event = pModEventFile->GetFirstSubKey();

	//This loops through all event_name keys
	while( cur_event )
	{
		//Create a mod events structure
		mod_event* pModEvent = new mod_event();

		//Add the event name it it
		V_strncpy( pModEvent->szEventName, cur_event->GetName(), EVENT_BUFFER_SIZE );

		DevMsg("[SPE]: Mod event name is %s.\n", pModEvent->szEventName);
		
		//Get the value to point to the first variable to this event.
		cur_field = cur_event->GetFirstSubKey();

		//This loops through an event's subkeys
		while( cur_field )
		{

			//Get the field name and type.
			char field_name[EVENT_BUFFER_SIZE];
			char field_type[EVENT_BUFFER_SIZE];
			
			V_strncpy( field_name, cur_field->GetName(), EVENT_BUFFER_SIZE );
			V_strncpy( field_type, cur_field->GetString(), EVENT_BUFFER_SIZE );

			DevMsg("[SPE]: Field name is %s.\n[SPE]: Field type is %s.\n", field_name, field_type);

			//Create a mod_variable structure
			mod_event_variable* pModVar = new mod_event_variable();

			//Store the information
			V_strncpy( pModVar->szVarName, field_name, EVENT_BUFFER_SIZE );
			V_strncpy( pModVar->szVarType, field_type, EVENT_BUFFER_SIZE );

			//Add it to our list.
			pModEvent->m_Vars.AddToTail( pModVar );

			cur_field = cur_field->GetNextKey();

		}

		//Add the mod event to our list
		m_Events.AddToTail( pModEvent );

		//Get next key
		cur_event = cur_event->GetNextKey();
	}

	DevMsg("[SPE]: Size of m_Events is %d!\n", m_Events.Count());

	//Get rid of our keyvalue
	pModEventFile->deleteThis();
}

//==================================================================================
// >> Returns a mod_event structure to an event name
//==================================================================================
mod_event* CModEventParser::findEvent( const char* szEventName )
{
	//Loop through our list
	for( int i = 0; i < m_Events.Count(); i++ )
	{
		//Compare names
		mod_event* temp = m_Events.Element( i );

		DevMsg("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
		DevMsg("[SPE]: Event name in findEvent is %s.\n", temp->szEventName);
		DevMsg("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

		if( strcmp( szEventName, temp->szEventName ) == 0 )
		{
			DevMsg("------------------------------------------------------------\n");
			DevMsg("[SPE]: Found event %s!\n", szEventName);
			DevMsg("------------------------------------------------------------\n");
			
			return temp;
		}
	}

	return NULL;
}
			
//==================================================================================
// >> Returns a python dictionary of variables based on a query.
//==================================================================================
PyObject* CModEventParser::getEventVariables(IGameEvent *pGameEvent)
{

	//Make sure the game event is valid
	if( !pGameEvent )
	{
		DevMsg("[SPE]: pGameEvent is invalid!\n");
		return NULL;
	}

	//Get the event name
	const char* szEventName = pGameEvent->GetName();

	DevMsg("****************************************\n");
	DevMsg("[SPE]: Event name %s\n", szEventName);
	DevMsg("****************************************\n");

	//Create dict to store event info
	PyObject* pDict = PyDict_New();

	//Find it in our dictionary
	mod_event* pEvent = findEvent( szEventName );

	//If it's valid, start parsing!
	if( pEvent )
	{
		//Loop through each of the event vars
		for( int i = 0; i < pEvent->m_Vars.Count(); i++ )
		{
			mod_event_variable* v = pEvent->m_Vars.Element( i );
			
			char szVarName[EVENT_BUFFER_SIZE];
			char szVarType[EVENT_BUFFER_SIZE];

			//Copy the variable names
			V_strncpy(szVarName, v->szVarName, EVENT_BUFFER_SIZE);
			V_strncpy(szVarType, v->szVarType, EVENT_BUFFER_SIZE);

			PyObject* val;

			DevMsg("[SPE]: VarName is %s!\n[SPE]: VarType is %s!\n", szVarName, szVarType);

			//Comparision statements!
			if( strcmp(szVarType, "short") == 0 )
			{
				int value = pGameEvent->GetInt(szVarName);
				val = Py_BuildValue("i", value);
			}

			else if( strcmp(szVarType, "string") == 0 )
			{
				val = Py_BuildValue("s", pGameEvent->GetString(szVarName));
			}

			else if( strcmp(szVarType, "float") == 0 )
			{
				float f = 0.0f;
				f = pGameEvent->GetFloat(szVarName);
				val = Py_BuildValue("f", f);
			}
			
			else if( strcmp(szVarType, "bool") == 0 )
			{
				int i = 0;
				i = pGameEvent->GetBool(szVarName);
				val = Py_BuildValue("i", i);
			}

			else if( strcmp(szVarType, "byte") == 0 )
			{
				int i = 0;
				i = pGameEvent->GetInt(szVarName);
				val = Py_BuildValue("i", i);
			}
			
			else if( strcmp(szVarType, "long") == 0 )
			{
				//TODO: Change this to long
				int i = 0;
				i = pGameEvent->GetInt(szVarName);
				val = Py_BuildValue("i", i);
			}

			//Now set the dict value!
			PyDict_SetItemString( pDict, szVarName, val );

			//Decrement the references
			Py_XDECREF( val );
		}
	
		return pDict;
	}
		
	DevMsg("[SPE]: pEvent was null.\n");
	return NULL;
}

