// ===================================================================
// Copyright (C) 2010 - Eventscripts Development Team
// File: es_hashtable.cpp
// Purpose: Implementation of CESHashTable.
// ===================================================================

// ===================================================================
// Includes
// ===================================================================
#include "spe_sigcache.h"
#include "spe_globals.h"

// ===================================================================
// These are all global hash tables.
// ===================================================================
CSPEHashTable g_SigTable;

// ===================================================================
// Constructor.
// ===================================================================
CSPEHashTable::CSPEHashTable() : m_SigTable(65536,0,0, &CSPEHashTable::HashDataCompare,
    &CSPEHashTable::HashKeyFunc)
{
    // Do nothing.
}

// ===================================================================
// Comparison function.
// ===================================================================
bool CSPEHashTable::HashDataCompare(const HashData_t &p1, const HashData_t &p2)
{
    return (FStrEq(p1.Signature, p2.Signature));
}

// ===================================================================
// Generates a hash for data.
// ===================================================================
unsigned int CSPEHashTable::HashKeyFunc(const HashData_t &p)
{
    const char* str = p.Signature;

    unsigned int hash = HASH_CONSTANT;
    int c;

    while( c = *str++ )
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

// ===================================================================
// Finds an element in the hash table.
// ===================================================================
void* CSPEHashTable::FindElement(const char* name)
{
    if( !name ) {
        return NULL;
    }

    // -----------------------------------------
    // Create a temp struct to find data.
    // -----------------------------------------
    HashData_t dataToFind;
    strcpy(dataToFind.Signature, name);
    dataToFind.Address = NULL;

    // -----------------------------------------
    // Have our hash table find it.
    // -----------------------------------------
    UtlHashHandle_t hKey = m_SigTable.Find(dataToFind);

    // -----------------------------------------
    // Make sure that the key is valid.
    // -----------------------------------------
    if( hKey != m_SigTable.InvalidHandle() )
    {
        // -----------------------------------------
        // Done!
        // -----------------------------------------
        return m_SigTable.Element(hKey).Address;
    }

    // -----------------------------------------
    // If we are here, it means hKey was
    // invalid.
    // -----------------------------------------
    return NULL;
}

// ===================================================================
// Inserts an element into the hash table.
// ===================================================================
void CSPEHashTable::InsertElement(const HashData_t *p)
{
    // -----------------------------------------
    // Insert the element into the hash table.
    // -----------------------------------------
    m_SigTable.Insert(*p);
}

// ===================================================================
// Removes an element from the hash table.
// ===================================================================
bool CSPEHashTable::RemoveElement(const char* name)
{
    if( !name ) {
        return false;
    }

    // -----------------------------------------
    // Create a temporary data struct to find
    // the hash data.
    // -----------------------------------------
    HashData_t dataToFind;
    strcpy(dataToFind.Signature, name);
    dataToFind.Address = NULL;

    // -----------------------------------------
    // Find a key to the data (if it exists).
    // -----------------------------------------
    UtlHashHandle_t hKey = m_SigTable.Find(dataToFind);

    // -----------------------------------------
    // Make sure it's valid.
    // -----------------------------------------
    if( hKey != m_SigTable.InvalidHandle() )
    {
        // -----------------------------------------
        // Remove it.
        // -----------------------------------------
        m_SigTable.Remove(hKey);

        // -----------------------------------------
        // Done!
        // -----------------------------------------
        return true;
    }

    // -----------------------------------------
    // If we're here, it means we didn't find
    // the data in the table.
    // -----------------------------------------
    return false;
}

