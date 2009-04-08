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

#ifndef ESX_GLOBALS_H
#define ESX_GLOBALS_H

/* Includes */
#include <eiface.h>
#include <iplayerinfo.h>
#include <ivoiceserver.h>
#include <filesystem.h>
#include "esx_hook_manager.h"

//==================================================================================
// >> Global Variables Class
//==================================================================================
class CGlobalManager
{
	public:
		CGlobalManager( IVEngineServer *pEngine, 
			IPlayerInfoManager *pInfoManager, 
			IVoiceServer* pVoice, 
			CSPEHookManager* HookMan, 
			IFileSystem* pSys);

		IVEngineServer* m_Engine;
		IPlayerInfoManager* m_Info;
		IVoiceServer* m_Voice;
		CSPEHookManager* m_Hooker;
		IFileSystem* m_Sys;
};

extern CGlobalManager* gGlobals;

#endif
