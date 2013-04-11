#ifndef _SPE_CALLBACK_H
#define _SPE_CALLBACK_H

// =======================================================================
// Includes
// =======================================================================

// -------------------------------------------
// STL includes
// -------------------------------------------
#include <utlvector.h>

// -------------------------------------------
// Python includes
// -------------------------------------------
#include "Python.h"

// -------------------------------------------
// Dyndetours includes
// -------------------------------------------
#include "dyndetours/callback_manager.h"

// =======================================================================
// Forward declarations
// =======================================================================
class CDetour;

// =======================================================================
// Namespaces to use
// =======================================================================
using namespace std;

// =======================================================================
// This class basically wraps up the stack parameters so that python can
// access and modify them.
// =======================================================================
class CPythonStack
{
    private:
        /* List of python objects containing the stack args. */
        PyObject* m_pArgList;

        /* Whether or not we're initialized. */
        bool      m_bOK;

    public:
        /* Constructor. */
        CPythonStack( CDetour* pDetour );
        ~CPythonStack( void );

        /* Reads arguments off the stack and then puts
         * them back onto the stack.
         */
        void ReadArgs( CDetour* pDetour );
        void PutArgs( CDetour* pDetour );

        /* Returns the arg pointer. */
        PyObject* GetArgs( void ) { return m_pArgList; }

        /* Returns the status of this object. */
        bool IsOK( void ) { return m_bOK; }
};

// =======================================================================
// A python callback instance is created for each hooked function. Only
// one can exist, and it handles pre and post calls for that function.
// =======================================================================
class CPythonCallback : public ICallbackManager
{
    private:
        /* Pre and post hooks. */
        CUtlVector<PyObject*> m_PreCalls;
        CUtlVector<PyObject*> m_PostCalls;

    public:
        /* ICallbackManager overrides. */
        virtual void Add( void* pFuncObj, eHookType type );
        virtual void Remove( void* pFuncObj, eHookType type );

        virtual HookRetBuf_t* DoPreCalls( CDetour* pDet );
        virtual HookRetBuf_t* DoPostCalls( CDetour* pDet );

        virtual const char* GetLang( void )
        {
            return "py";
        }

};

#endif // _SPE_CALLBACK_H