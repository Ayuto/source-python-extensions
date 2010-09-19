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
#include "spe_event_parser.h"
#include "spe_globals.h"
#include <KeyValues.h>

//=================================================================================
// Parser global.
//=================================================================================
CEventParser* g_pParser;

//=================================================================================
// Constructor.
//=================================================================================
CEventParser::CEventParser( void )
{
	// ------------------------------------
	// Get the current mod game dir path.
	// ------------------------------------
	engine->GetGameDir(m_szGameDir, MAX_STRING_LEN);

	// ------------------------------------
	// Parse events from these standard
	// files.
	// ------------------------------------
	ParseEventsFromFile("resource/modevents.res");
	ParseEventsFromFile("../hl2/resource/gameevents.res");
	ParseEventsFromFile("../hl2/resource/hltvevents.res");
	ParseEventsFromFile("../hl2/resource/serverevents.res");
}

//=================================================================================
// Destructor.
//=================================================================================
CEventParser::~CEventParser( void )
{
	// ------------------------------------
	// Free up all the memory occupied by
	// the data structs.
	// ------------------------------------
	FOR_EACH_VEC( m_EventData, i )
	{
		EventData_t* pData = m_EventData[i];
		
		pData->eventVars->PurgeAndDeleteElements();
		delete pData->eventVars;
		delete pData;
	}
}

//=================================================================================
// Parses events from a .res file.
//=================================================================================
bool CEventParser::ParseEventsFromFile( const char* szPath )
{
	// ------------------------------------
	// Make sure the path is valid.
	// ------------------------------------
	if( !szPath ) {
		return false;
	}

	// ------------------------------------
	// Print both the game dir and the
	// given path to a string.
	// ------------------------------------
	char szFullPath[MAX_STRING_LEN * 2];
	V_snprintf(szFullPath, MAX_STRING_LEN * 2, "%s/%s", m_szGameDir, szPath);

	// ------------------------------------
	// Print out the path.
	// ------------------------------------
	DevMsg(1, "[SPE]: Full path to res file is %s\n", szFullPath);

	// ------------------------------------
	// Load the file up as a keyvalues
	// instance.
	// ------------------------------------
	KeyValues* pResFile = new KeyValues("resfile");
	bool bResult = pResFile->LoadFromFile(filesystem, szFullPath);

	// ------------------------------------
	// Make sure the operation was successful.
	// ------------------------------------
	if( bResult == false ) {
		return false;
	}

	// ------------------------------------
	// Loop through each key and it's sub
	// keys.
	// ------------------------------------
	KeyValues* pCurKey = pResFile->GetFirstSubKey();
	KeyValues* pCurSubKey = NULL;

	while( pCurKey )
	{
		// ------------------------------------
		// Print out debug info.
		// ------------------------------------
		DevMsg(1, "[SPE]: Found event %s. Creating struct!\n", pCurKey->GetName());

		// ------------------------------------
		// Create an Event data struct.
		// ------------------------------------
		EventData_t* pData = new EventData_t;
		pData->eventVars = new CUtlLinkedList<EventVariable_t*>();

		// ------------------------------------
		// Copy over the name of the event.
		// ------------------------------------
		V_strncpy(pData->szEventName, pCurKey->GetName(), MAX_STRING_LEN);

		// ------------------------------------
		// Now loop through each subkey of this 
		// event.
		// ------------------------------------
		pCurSubKey = pCurKey->GetFirstSubKey();

		// ------------------------------------
		// Loop through each subkey until we hit
		// null.
		// ------------------------------------
		while( pCurSubKey )
		{
			// ------------------------------------
			// Print debug info.
			// ------------------------------------
			DevMsg(1, "[SPE]: Found event_var %s of type %s!\n", pCurSubKey->GetName(),
				pCurSubKey->GetString());

			// ------------------------------------
			// Create an event var struct.
			// ------------------------------------
			EventVariable_t* pVar = new EventVariable_t;

			// ------------------------------------
			// Copy over the name and value.
			// ------------------------------------
			V_strncpy(pVar->szName, pCurSubKey->GetName(), MAX_STRING_LEN);
			V_strncpy(pVar->szType, pCurSubKey->GetString(), MAX_STRING_LEN);

			// ------------------------------------
			// Debug message.
			// ------------------------------------
			DevMsg(1, "Adding event information to event data!\n");

			// ------------------------------------
			// Add it to the eventdata struct.
			// ------------------------------------
			pData->eventVars->AddToTail(pVar);

			// ------------------------------------
			// Debug message.
			// ------------------------------------
			DevMsg(1, "Moving onto the next variable sub-key.\n");

			// ------------------------------------
			// Move onto the next event variable.
			// ------------------------------------
			pCurSubKey = pCurSubKey->GetNextKey();
		}

		// ------------------------------------
		// Debug message.
		// ------------------------------------
		DevMsg(1, "Inserting completed EventData struct into event hooks!\n");

		// ------------------------------------
		// Now add the event data to our
		// hashed list.
		// ------------------------------------
		m_EventData.AddToTail( pData );

		// ------------------------------------
		// Debug message.
		// ------------------------------------
		DevMsg(1, "Moving onto the next event..\n");

		// ------------------------------------
		// Move onto the next key.
		// ------------------------------------
		pCurKey = pCurKey->GetNextKey();
	}

	// ------------------------------------
	// Free up keyvalues memory.
	// ------------------------------------
	pResFile->deleteThis();

	// ------------------------------------
	// We're done!
	// ------------------------------------
	return true;
}

//=================================================================================
// Returns an event's layout.
//=================================================================================
EventData_t* CEventParser::FindEventData( const char* szEventName )
{
	// ------------------------------------
	// Make sure the event name is valid.
	// ------------------------------------
	if( !szEventName ) {
		DevMsg(1, "[SPE]: szEventName was null.\n");
		return NULL;
	}

	// ------------------------------------
	// Search for it in our list.
	// ------------------------------------
	FOR_EACH_VEC( m_EventData, i )
	{
		// ------------------------------------
		// Get the eventdata_t struct.
		// ------------------------------------
		EventData_t* pData = m_EventData[i];

		// ------------------------------------
		// Compare names
		// ------------------------------------
		if( pData && (FStrEq(pData->szEventName, szEventName) == true))
		{
			DevMsg(1, "[SPE]: Found %s at index %d!\n", pData->szEventName, i);
			return pData;
		}
	}
	
	// ------------------------------------
	// Didn't find it if we are here.
	// ------------------------------------
	DevMsg(1, "[SPE]: Could not find %s in the event list!\n", szEventName);
	return NULL;
}

//==================================================================================
// Returns a python dictionary of variables based on a query.
//==================================================================================
PyObject* CEventParser::GetEventVariables( IGameEvent *pGameEvent )
{
	// ------------------------------------
	// Make sure the game event is valid.
	// ------------------------------------
	if( !pGameEvent )
	{
		DevMsg(2, "[SPE]: [getEventVariables] -> pGameEvent is invalid!\n");
		return NULL;
	}

	// ------------------------------------
	// Get the event name.
	// ------------------------------------
	const char* szEventName = pGameEvent->GetName();
	DevMsg(2, "[SPE]: [getEventVariables] -> Event name %s\n", szEventName);

	// ------------------------------------
	// Create dict to store event info
	// ------------------------------------
	PyObject* pDict = PyDict_New();

	// ------------------------------------
	// Find it in our dictionary.
	// ------------------------------------
	EventData_t* pEvent = FindEventData( szEventName );

	// ------------------------------------
	// If it's valid, start parsing!
	// ------------------------------------
	if( pEvent )
	{
		// ------------------------------------
		// Get the event name into a python 
		// object.
		// ------------------------------------
		PyObject* pEventName = Py_BuildValue("s", pEvent->szEventName);

		// ------------------------------------
		// Create a key for the event in the
		// dict.
		// ------------------------------------
		PyObject* pEventKey = Py_BuildValue("s", "spe_eventname");

		// ------------------------------------
		// Set the event name.
		// ------------------------------------
		PyDict_SetItem(pDict, pEventKey, pEventName);

		// ------------------------------------
		// Decrement references.
		// ------------------------------------
		Py_DECREF( pEventName );
		Py_DECREF( pEventKey );

		// ------------------------------------
		// Loop through each of the event vars.
		// ------------------------------------
		for( int i = 0; i < pEvent->eventVars->Count(); i++ )
		{
			// ------------------------------------
			// Grab the event data from the list.
			// ------------------------------------
			EventVariable_t* v = pEvent->eventVars->Element( i );

			char szVarName[MAX_STRING_LEN];
			char szVarType[MAX_STRING_LEN];

			// ------------------------------------
			// Copy the variable name and type.
			// ------------------------------------
			V_strncpy(szVarName, v->szName, MAX_STRING_LEN);
			V_strncpy(szVarType, v->szType, MAX_STRING_LEN);

			// ------------------------------------
			// This will convert the value from
			// the game event instance to a python
			// type.
			// ------------------------------------
			PyObject* val = NULL;
			DevMsg(2, "[SPE]: [getEventVariables] -> VarName is %s!\n[SPE]: [getEventVariables] -> VarType is %s!\n", szVarName, szVarType);

			// ------------------------------------
			// Figure out what to parse the
			// event variable to.
			// ------------------------------------
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
				// ------------------------------------
				// TODO: Change this to long
				// ------------------------------------
				int i = 0;
				i = pGameEvent->GetInt(szVarName);
				val = Py_BuildValue("i", i);
				DevMsg("[SPE]: [getEventVariables] -> Building a long.\n");
			}

			// ------------------------------------
			// Now set the dict value!
			// ------------------------------------
			DevMsg("[SPE]: [getEventVariables] -> Setting the item in the dict.\n");
			PyDict_SetItemString( pDict, szVarName, val );

			// ------------------------------------
			// Decrement the reference to the value.
			// ------------------------------------
			Py_DECREF(val);
		}

		// ------------------------------------
		// Done.
		// ------------------------------------
		return pDict;
	}

	DevMsg(2, "[SPE]: [getEventVariables] -> pEvent was null.\n");
	return NULL;
}

