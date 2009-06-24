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

#ifndef SPE_EVENT_PARSER_H
#define SPE_EVENT_PARSER_H

#include <Python.h>
#include <utlvector.h>
#include <igameevents.h>

// Buffer size
#define EVENT_BUFFER_SIZE 255

//==================================================================================
// >> Mod event variable
//==================================================================================
struct EventVar_t
{
	char szVarName[EVENT_BUFFER_SIZE];
	char szVarType[EVENT_BUFFER_SIZE];
};

//==================================================================================
// >> Facilitates information storage in python objects.
//==================================================================================
struct ModEvent_t
{
	char szEventName[EVENT_BUFFER_SIZE];
	CUtlVector<EventVar_t*> m_Vars;
};

//==================================================================================
// >> ModEvent parsing class
//==================================================================================
class CModEventParser
{
private:
	CUtlVector<ModEvent_t*> m_Events; //Stores all mod events.

public:
	CModEventParser();

	void		  ParseEvents( const char* szResFilePath );
	ModEvent_t*	  FindEvent( const char* szEventName );
	PyObject*	  GetEventVariables( IGameEvent* pGameEvent );
};

extern CModEventParser* g_pParser;

#endif
