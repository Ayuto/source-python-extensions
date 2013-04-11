/**
* =============================================================================
* Source Python Extensions
* Copyright (C) 2011 Deniz "your-name-here" Sezen.  All rights reserved.
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
#ifndef _SPE_SIGCACHE_H
#define _SPE_SIGCACHE_H

// ==========================================================================
// Code adapted from es_hashtable.h from Mattie Casper's Eventscripts.
// ==========================================================================

// ==========================================================================
// Includes
// ==========================================================================
#include <utlhash.h>

// ==========================================================================
// Hash constant.
// ==========================================================================
#define HASH_CONSTANT 5381

// ==========================================================================
// Hash data object.
// ==========================================================================
struct HashData_t
{
    char        Signature[256];
    void*       Address;
};

// ==========================================================================
// The hashtable class.
// ==========================================================================
class CSPEHashTable
{
private:
    /* This will store our data. */
    CUtlHash<HashData_t> m_SigTable;

public:
    // -----------------------------------------
    // @brief Constructor.
    // -----------------------------------------
    CSPEHashTable( void );

    // -----------------------------------------
    // @brief The hash compare function.
    // -----------------------------------------
    static bool HashDataCompare(const HashData_t &p1,
        const HashData_t &p2);

    // -----------------------------------------
    // @brief Returns a key to the given data.
    // -----------------------------------------
    static unsigned int HashKeyFunc(const HashData_t &p);

    // -----------------------------------------
    // @brief Returns a value by name.
    // -----------------------------------------
    void* FindElement(const char* name);

    // -----------------------------------------
    // @brief Inserts an element into the hash
    //  table.
    // -----------------------------------------
    void InsertElement(const HashData_t *p);

    // -----------------------------------------
    // @brief Removes an element by name.
    // -----------------------------------------
    bool RemoveElement(const char* name);
};

// ===================================================================
// These are all global hash tables.
// ===================================================================
extern CSPEHashTable g_SigTable;


#endif // _SPE_SIGCACHE_H