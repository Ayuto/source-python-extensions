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
#include "spe_globals.h"
#include "spe_python.h"
#include "spe_scanner.h"
#ifdef _LINUX
#   include <fcntl.h>
#   include <link.h>
#   include <sys/mman.h>
#endif

//=================================================================================
// Creates and returns a pointer to some space
//=================================================================================
DECLARE_PYCMD( alloc, "Allocates space and returns a pointer to it." )
{
	int iBytesToAlloc;
	void *pAlloc = NULL;

	if( !PyArg_ParseTuple(args, "i", &iBytesToAlloc ) )
	{
		DevMsg("[SPE]: spe_alloc: Could not parse arguments.\n");
		return Py_BuildValue("");
	}

	pAlloc = malloc( iBytesToAlloc );

	if( !pAlloc )
	{
		DevMsg("[SPE]: Could not create memory!\n");
		return Py_BuildValue("");
	}

	return Py_BuildValue("i", pAlloc);
}

//=================================================================================
// Deallocates passed in memory
//=================================================================================
DECLARE_PYCMD( dealloc, "Deallocates memory" )
{
	void *addr = NULL;

	if( !PyArg_ParseTuple(args, "i", &addr) )
	{
		DevMsg("[SPE]: spe_alloc: Could not parse arguments.\n");
		return Py_BuildValue("");
	}
	
	if( !addr )
		return Py_BuildValue("");

	free( addr );

	return Py_BuildValue("");
}

//=================================================================================
// Modifies memory at a particular memory location
//=================================================================================
DECLARE_PYCMD( setLocVal, "Sets the contents of a particular memory location" )
{
	char      type = NULL;
	void*     addr = NULL;
	PyObject* val  = NULL;

	if( !PyArg_ParseTuple(args, "ciO", &type, &addr, &val) )
	{
		DevMsg("[SPE]: spe_setLocVal: Can't parse arguments.\n");
		return Py_BuildValue("");
	}

	if(!addr)
		return Py_BuildValue("");

	switch(type)
	{
		case DC_SIGCHAR_BOOL:
		{
			bool b;
			PyArg_ParseTuple( val, "i", &b );
			*(bool *)((char *)addr) = b;
		}

		case DC_SIGCHAR_INT:
		{	
			int v = PyInt_AsLong( val );
			*(int *)((char *)addr) = v;
		} break;

		case DC_SIGCHAR_POINTER:
		{
			DCpointer ptr;
			ptr = (DCpointer) ( (DCint) PyLong_AsLongLong(val) );
			*(DCpointer *)((char *)addr) = ptr;
		} break;

		case DC_SIGCHAR_FLOAT:
		{
			float f;
			f = (float) PyFloat_AsDouble( val );
			*(float *)((char *)addr) = f;
		} break;

		default:
		{
			DevMsg("[SPE]: setLocVal: A valid type indicator was not passed in!\n");
		} break;
	}

	return Py_BuildValue("");
}

//=================================================================================
// Returns what is stored at a particular memory location
//=================================================================================
DECLARE_PYCMD( getLocVal, "Sets the contents of a particular memory location" )
{
	char      type = NULL;
	void*     addr = NULL;

	if( !PyArg_ParseTuple(args, "ci", &type, &addr) )
	{
		DevMsg("[SPE]: spe_setLocVal: Can't parse arguments.\n");
		return Py_BuildValue("");
	}

	if(!addr)
		return Py_BuildValue("");

	switch(type)
	{
		case DC_SIGCHAR_BOOL:
			return Py_BuildValue("b", *(bool *)((char *)addr));

		case DC_SIGCHAR_INT:
			return Py_BuildValue("i", *(int *)((char *)addr));
		
		case DC_SIGCHAR_POINTER:
		{
			void* temp = NULL;
			memcpy(temp, *(DCpointer *)((char *)addr), sizeof(char *));
			return Py_BuildValue("i", temp);
		};

		case DC_SIGCHAR_FLOAT:
			return Py_BuildValue("f", *(float *)((char *)addr));
		
		default:
		{
			DevMsg("[SPE]: setLocVal: A valid type indicator was not passed in!\n");
		} break;
	}

	return Py_BuildValue("");
}


//=================================================================================
// Returns the address of a signature.
//=================================================================================
#ifdef _WIN32
DECLARE_PYCMD( findFunction, "Finds and returns the address of a signature." )
{
    // We need to store the identifier ( hex / symbol ).
    PyObject *pIdent;
    
    // And if on windows, a signature length
    int      iSigLength;
    
    // Parse the tuple
    if( !PyArg_ParseTuple(args, "Oi", &pIdent, &iSigLength) )
    {
        DevMsg("[SPE]: Could not parse the tuple in spe_findFunction.\n");
        return Py_BuildValue("");
    }

    return Py_BuildValue("i", (int)g_SigMngr.ResolveSig(laddr, PyString_AsString(pIdent), iSigLength));
}

#else
//=================================================================================
// Returns the address of a symbol.
//=================================================================================
DECLARE_PYCMD( findSymbol, "Returns the address of a symbol." )
{
    // Parse out symbol
    char* symbol;

    if( !PyArg_ParseTuple(args, "s", &symbol) )
    {
        DevMsg("[SPE]: spe_findSymbol: Couldn't parse function arguments.\n");
        return Py_BuildValue("");
    }

    // -----------------------------------------
    // We need to use mmap now that VALVe has
    // made them all private! Damn you gabe!
    // Thank you to DamagedSoul from AlliedMods
    // for the following code.
    // It can be found at:
    // http://hg.alliedmods.net/sourcemod-central/file/dc361050274d/core/logic/MemoryUtils.cpp
    // -----------------------------------------
    struct link_map *dlmap;
	struct stat dlstat;
	int dlfile;
	uintptr_t map_base;
	Elf32_Ehdr *file_hdr;
	Elf32_Shdr *sections, *shstrtab_hdr, *symtab_hdr, *strtab_hdr;
	Elf32_Sym *symtab;
	const char *shstrtab, *strtab;
	uint16_t section_count;
	uint32_t symbol_count;

	dlmap = (struct link_map *)laddr;
	symtab_hdr = NULL;
	strtab_hdr = NULL;

	dlfile = open(dlmap->l_name, O_RDONLY);
	if (dlfile == -1 || fstat(dlfile, &dlstat) == -1)
	{
        Msg("dlfile == -1!\n");
		close(dlfile);
		return NULL;
	}

	/* Map library file into memory */
	file_hdr = (Elf32_Ehdr *)mmap(NULL, dlstat.st_size, PROT_READ, MAP_PRIVATE, dlfile, 0);
	map_base = (uintptr_t)file_hdr;
	if (file_hdr == MAP_FAILED)
	{
        Msg("file_hdr == MAP_FAILED!\n");
		close(dlfile);
		return NULL;
	}
	close(dlfile);

	if (file_hdr->e_shoff == 0 || file_hdr->e_shstrndx == SHN_UNDEF)
	{
        Msg("file_hdr->e_shoff == 0!\n");
		munmap(file_hdr, dlstat.st_size);
		return NULL;
	}

	sections = (Elf32_Shdr *)(map_base + file_hdr->e_shoff);
	section_count = file_hdr->e_shnum;
	/* Get ELF section header string table */
	shstrtab_hdr = &sections[file_hdr->e_shstrndx];
	shstrtab = (const char *)(map_base + shstrtab_hdr->sh_offset);

	/* Iterate sections while looking for ELF symbol table and string table */
	for (uint16_t i = 0; i < section_count; i++)
	{
		Elf32_Shdr &hdr = sections[i];
		const char *section_name = shstrtab + hdr.sh_name;

		if (strcmp(section_name, ".symtab") == 0)
		{
			symtab_hdr = &hdr;
		}
		else if (strcmp(section_name, ".strtab") == 0)
		{
			strtab_hdr = &hdr;
		}
	}

	/* Uh oh, we don't have a symbol table or a string table */
	if (symtab_hdr == NULL || strtab_hdr == NULL)
	{
        Msg("File doesn't have a symbol table!\n");
		munmap(file_hdr, dlstat.st_size);
		return NULL;
	}

	symtab = (Elf32_Sym *)(map_base + symtab_hdr->sh_offset);
	strtab = (const char *)(map_base + strtab_hdr->sh_offset);
	symbol_count = symtab_hdr->sh_size / symtab_hdr->sh_entsize;
    void* sym_addr = NULL;

	/* Iterate symbol table starting from the position we were at last time */
	for (uint32_t i = 0; i < symbol_count; i++)
	{
		Elf32_Sym &sym = symtab[i];
		unsigned char sym_type = ELF32_ST_TYPE(sym.st_info);
		const char *sym_name = strtab + sym.st_name;

		/* Skip symbols that are undefined or do not refer to functions or objects */
		if (sym.st_shndx == SHN_UNDEF || (sym_type != STT_FUNC && sym_type != STT_OBJECT))
		{
			continue;
		}

		if (strcmp(symbol, sym_name) == 0)
		{
            sym_addr = (void *)(dlmap->l_addr + sym.st_value);
			break;
		}
	}

    // Unmap the file now.
	munmap(file_hdr, dlstat.st_size);

    // Validate it
    if( !sym_addr )
    {
        DevMsg("[SPE]: Could not find symbol %s!\n", symbol);
        return Py_BuildValue("");
    }

    // Print it out
    DevMsg("************************************\n");
    DevMsg("[SPE]: Symbol: %s.\n", symbol);
    DevMsg("[SPE]: Symbol address: %d.\n", sym_addr);
    DevMsg("************************************\n");

    // Return it
    return Py_BuildValue("i", (int)sym_addr);
}

#endif // _WIN32

