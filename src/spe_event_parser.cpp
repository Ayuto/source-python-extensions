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

#include "spe_event_parser.h"
#include "spe_globals.h"
#include <KeyValues.h>

CModEventParser* g_pParser;

//==================================================================================
// >> Constructor
//==================================================================================
CModEventParser::CModEventParser()
{
	//Parse out game specific events.
	ParseEvents("resource/ModEvents.res");

#if defined ENGINE_LEFT4DEAD
	ParseEvents("resource/serverevents.res");
	ParseEvents("resource/gameevents.res");
#endif

	ParseEvents("../hl2/resource/gameevents.res");
	ParseEvents("../hl2/resource/hltvevents.res");
	ParseEvents("../hl2/resource/serverevents.res");
}

//==================================================================================
// >> Parses out the events for each mod
//==================================================================================
void CModEventParser::ParseEvents(const char* szResFilePath)
{
	KeyValues* pModEventFile = new KeyValues("events");

	//Load the mod event file
	if( !pModEventFile->LoadFromFile(filesystem, szResFilePath) )
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
		ModEvent_t* pModEvent = new ModEvent_t();

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
			EventVar_t* pModVar = new EventVar_t();

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

	DevMsg("[SPE]: Total number of parsed events is %d!\n", m_Events.Count());

	//Get rid of our keyvalue
	pModEventFile->deleteThis();
}

//==================================================================================
// >> Returns a mod_event structure to an event name
//==================================================================================
ModEvent_t* CModEventParser::FindEvent( const char* szEventName )
{
	//Loop through our list
	for( int i = 0; i < m_Events.Count(); i++ )
	{
		//Compare names
		ModEvent_t* temp = m_Events.Element( i );

		if( strcmp( szEventName, temp->szEventName ) == 0 )
		{
			DevMsg("------------------------------------------------------------\n");
			DevMsg("[SPE]: Found event %s!\n", szEventName);
			DevMsg("------------------------------------------------------------\n");

			return temp;
		}
	}

	DevMsg("[SPE]: Could not find a mod_event structure with the name of %s.\n", szEventName);
	return NULL;
}

//==================================================================================
// >> Returns a python dictionary of variables based on a query.
//==================================================================================
PyObject* CModEventParser::GetEventVariables(IGameEvent *pGameEvent)
{

	//Make sure the game event is valid
	if( !pGameEvent )
	{
		DevMsg("[SPE]: [getEventVariables] -> pGameEvent is invalid!\n");
		return NULL;
	}

	//Get the event name
	const char* szEventName = pGameEvent->GetName();

	DevMsg("[SPE]: [getEventVariables] -> Event name %s\n", szEventName);

	//Create dict to store event info
	PyObject* pDict = PyDict_New();

	//Find it in our dictionary
	ModEvent_t* pEvent = FindEvent( szEventName );

	//If it's valid, start parsing!
	if( pEvent )
	{
		// Set event name
		PyDict_SetItemString(pDict, "spe_eventname", Py_BuildValue("s", pEvent->szEventName));

		//Loop through each of the event vars
		for( int i = 0; i < pEvent->m_Vars.Count(); i++ )
		{
			EventVar_t* v = pEvent->m_Vars.Element( i );

			char szVarName[EVENT_BUFFER_SIZE];
			char szVarType[EVENT_BUFFER_SIZE];

			//Copy the variable names
			V_strncpy(szVarName, v->szVarName, EVENT_BUFFER_SIZE);
			V_strncpy(szVarType, v->szVarType, EVENT_BUFFER_SIZE);

			PyObject* val;

			DevMsg("[SPE]: [getEventVariables] -> VarName is %s!\n[SPE]: [getEventVariables] -> VarType is %s!\n", szVarName, szVarType);

			//Comparision statements!
			if( strcmp(szVarType, "short") == 0 )
			{
				int v = pGameEvent->GetInt(szVarName);
				val = Py_BuildValue("i", v);
				DevMsg("[SPE]: [getEventVariables] -> Building an integer.\n");
			}

			else if( strcmp(szVarType, "string") == 0 )
			{
				val = Py_BuildValue("s", pGameEvent->GetString(szVarName));
				DevMsg("[SPE]: [getEventVariables] -> Building a string.\n");
			}

			else if( strcmp(szVarType, "float") == 0 )
			{
				float f = 0.0f;
				f = pGameEvent->GetFloat(szVarName);
				val = Py_BuildValue("f", f);
				DevMsg("[SPE]: [getEventVariables] -> Building a float.\n");
			}

			else if( strcmp(szVarType, "bool") == 0 )
			{
				int i = 0;
				i = pGameEvent->GetBool(szVarName);
				val = Py_BuildValue("i", i);
				DevMsg("[SPE]: [getEventVariables] -> Building a boolean.\n");
			}

			else if( strcmp(szVarType, "byte") == 0 )
			{
				int i = 0;
				i = pGameEvent->GetInt(szVarName);
				val = Py_BuildValue("i", i);
				DevMsg("[SPE]: [getEventVariables] -> Building a byte.\n");
			}

			else if( strcmp(szVarType, "long") == 0 )
			{
				//TODO: Change this to long
				int i = 0;
				i = pGameEvent->GetInt(szVarName);
				val = Py_BuildValue("i", i);
				DevMsg("[SPE]: [getEventVariables] -> Building a long.\n");
			}

			//Now set the dict value!
			DevMsg("[SPE]: [getEventVariables] -> Setting the item in the dict.\n");
			PyDict_SetItemString( pDict, szVarName, val );
		}

		return pDict;
	}

	DevMsg("[SPE]: [getEventVariables] -> pEvent was null.\n");
	return NULL;
}
