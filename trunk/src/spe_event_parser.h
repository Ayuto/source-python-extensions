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

#ifndef _SPE_EVENT_PARSER_H
#define _SPE_EVENT_PARSER_H

//=================================================================================
// Purpose: Parses Source Engine event .res files into data structures for
//  use with SPE.
//=================================================================================

//=================================================================================
// Includes
//=================================================================================
#include <Python.h>
#include <utlvector.h>
#include <utllinkedlist.h>
#include "spe_globals.h"

//=================================================================================
// Represents a single event variable.
//=================================================================================
struct EventVariable_t
{
	char szName[MAX_STRING_LEN];
	char szType[MAX_STRING_LEN];
};

//=================================================================================
// Represents a single event.
//=================================================================================
struct EventData_t
{
	char szEventName[MAX_STRING_LEN];
	CUtlLinkedList<EventVariable_t*>* eventVars;
};

//=================================================================================
// The event parser class.
//=================================================================================
class CEventParser
{
	private:
		// ------------------------------------
		// The path to the current mod's game
		// directory.
		// ------------------------------------
		char m_szGameDir[MAX_STRING_LEN];

		// ------------------------------------
		// List of event information.
		// ------------------------------------
		CUtlVector<EventData_t *> m_EventData;

	public:

		// ------------------------------------
		// @brief Constructor
		// ------------------------------------
		CEventParser( void );

		// ------------------------------------
		// @brief Destructor.
		// ------------------------------------
		~CEventParser( void );

		// ------------------------------------
		// @brief Parses event information
		//	from the given file.
		// @param szPath - Path to the res file
		//	relative to the mod game directory
		//	(i.e. cstrike).
		// ------------------------------------
		bool ParseEventsFromFile( const char* szPath );

		// ------------------------------------
		// @brief Returns an event struct
		//	based on the given event name.
		// @param szEventName - The event
		//	whose representation to retrieve.
		// @return The EventData_t struct
		//	associated with this event. NULL
		//	if it was not found.
		// ------------------------------------
		EventData_t* FindEventData( const char* szEventName );

		// ------------------------------------
		// @brief Retrieves the event data
		//	in a python useable form.
		// @param pGameEvent - The game event
		//	from which to parse data.
		// @return Python dict to said data.
		// ------------------------------------
		PyObject* GetEventVariables( IGameEvent *pGameEvent );
};

//=================================================================================
// Global variable for the parser.
//=================================================================================
extern CEventParser* g_pParser;

#endif

