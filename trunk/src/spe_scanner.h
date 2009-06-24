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

#ifndef _INCLUDE_CSIGMNGR_H
#define _INCLUDE_CSIGMNGR_H

struct signature_t
{
	void *allocBase;
	void *memInBase;
	int  memSize;
	void *offset;
	const char *sig;
	int siglen;
};

class CSigMngr
{
public:
	void *ResolveSig(void *memInBase, const char *pattern, int siglen);
	int ResolvePattern(void *memInBase, const char *pattern, int siglen, int number, ...);
private:
	bool ResolveAddress(signature_t *sigmem);
};

extern CSigMngr g_SigMngr;

#endif //_INCLUDE_CSIGMNGR_H

