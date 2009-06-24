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

#ifdef _WIN32
#define WINDOWS_LEAN_AND_MEAN   1
#include <windows.h>
#else
#include <dlfcn.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "spe_scanner.h"

CSigMngr g_SigMngr;

bool CSigMngr::ResolveAddress(signature_t *sigmem)
{
#ifdef WIN32
	MEMORY_BASIC_INFORMATION mem;

	if (!VirtualQuery(sigmem->memInBase, &mem, sizeof(MEMORY_BASIC_INFORMATION)))
		return false;

	if (mem.AllocationBase == NULL)
		return false;

	HMODULE dll = (HMODULE)mem.AllocationBase;

	//code adapted from hullu's linkent patch
	union 
	{
		unsigned long mem;
		IMAGE_DOS_HEADER *dos;
		IMAGE_NT_HEADERS *pe;
	} dllmem;

	dllmem.mem = (unsigned long)dll;

	if (IsBadReadPtr(dllmem.dos, sizeof(IMAGE_DOS_HEADER)) || (dllmem.dos->e_magic != IMAGE_DOS_SIGNATURE))
		return false;

	dllmem.mem = ((unsigned long)dll + (unsigned long)(dllmem.dos->e_lfanew));
	if (IsBadReadPtr(dllmem.pe, sizeof(IMAGE_NT_HEADERS)) || (dllmem.pe->Signature != IMAGE_NT_SIGNATURE))
		return false;

	//end adapted hullu's code

	IMAGE_NT_HEADERS *pe = dllmem.pe;

	sigmem->allocBase = mem.AllocationBase;
	sigmem->memSize = (DWORD)(pe->OptionalHeader.SizeOfImage);

	return true;
#else
	Dl_info info;

	if (!dladdr(sigmem->memInBase, &info))
		return false;

	if (!info.dli_fbase || !info.dli_fname)
		return false;

	sigmem->allocBase = info.dli_fbase;

	pid_t pid = getpid();
	char file[255];
	char buffer[2048];
	snprintf(file, sizeof(file)-1, "/proc/%d/maps", pid);
	FILE *fp = fopen(file, "rt");
	if (!fp)
		return false;
	void *start=NULL;
	void *end=NULL;
	void *found=NULL;
	while (!feof(fp))
	{
		fgets(buffer, sizeof(buffer)-1, fp);
#if defined AMD64
		sscanf(buffer, "%Lx-%Lx", &start, &end);
#else
		sscanf(buffer, "%lx-%lx", &start, &end);
#endif

		if (start == sigmem->allocBase)
		{
			found = end;
			break;
		}
	}
	fclose(fp);

	if (!found)
		return false;

	sigmem->memSize = (unsigned long)end - (unsigned long)start;

#ifdef DEBUG
	DevMsg("Alloc base: %p\n", sigmem->allocBase);
#endif

	return true;
#endif
}

void *CSigMngr::ResolveSig(void *memInBase, const char *pattern, int siglen)
{
	signature_t sig;

	memset(&sig, 0, sizeof(signature_t));

	sig.sig = (const char *)pattern;
	sig.siglen = siglen;
	sig.memInBase = memInBase;

	if (!ResolveAddress(&sig))
		return NULL;

	const char *paddr = (const char *)sig.allocBase;
	bool found;

	register unsigned int j;

	sig.memSize -= sig.siglen;      //prevent a crash maybe?

	for (int i=0; i<sig.memSize; i+=sizeof(unsigned long *))
	{
		found = true;
		for (j=0; j<sig.siglen; j++)
		{
			if ( (pattern[j] != (char)0x2A) &&
				(pattern[j] != paddr[j]) )
			{
				found = false;
				break;
			}
		}
		if (found)
		{
			sig.offset = (void *)paddr;
			break;
		}
		//we're always gonna be on a four byte boundary
		paddr += sizeof(unsigned long *);
	}

	return sig.offset;
}

