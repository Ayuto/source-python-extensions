// =======================================================================
// Includes
// =======================================================================

// -------------------------------------------
// PyGameLib includes
// -------------------------------------------
#include "spe_callback.h"

// -------------------------------------------
// Dyndetours includes
// -------------------------------------------
#include "dyndetours/detour_class.h"
#include "dyndetours/register_class.h"
#include "dyndetours/conv_interface.h"
#include "dyndetours/func_class.h"
#include "dyndetours/func_stack.h"
#include "dyndetours/func_types.h"

// =======================================================================
// Fixes for linux
// =======================================================================
#ifdef __linux__
#  define INT32 int32_t
#endif

// =======================================================================
// Reads an argument out of ESP.
// TODO: Fix this to work with non-atomic data types.
// =======================================================================
template<class T>
T ReadArg( void* addr )
{
    return *((T *)(addr));
}

// =======================================================================
// Places an argument back into the stack.
// TODO: Fix this to work with non-atomic data types.
// =======================================================================
template<class T>
void SetArg( void* addr, T value )
{
    *((T *)addr) = value;
}

// =======================================================================
// Reads arguments off the stack.
// =======================================================================
void CPythonStack::ReadArgs( CDetour* pDetour )
{
    /* Grab the function object. */
    CFuncObj* pFuncObj = pDetour->GetFuncObj();

    /* Make sure it is valid. */
    if( !pFuncObj )
        return;

    /* Get the stack object. */
    CFuncStack* pStk = pFuncObj->GetStack();

    /* Make sure that is valid too. */
    if( !pStk )
        return;

    /* Get the calling convention object of the function. */
    ICallConvention* pConv = pDetour->GetAsmBridge()->GetConv();

    /* Make sure that is valid. */
    if( !pConv )
        return;

    /* Get the register object. */
    CRegisterObj* pRegisters = pConv->GetRegisters();

    /* Make sure this is valid. */
    if( !pRegisters )
        return;

    /* Now create a python list object. */
    m_pArgList = PyList_New(0);

    /* Make sure it's valid. */
    if( !m_pArgList )
        return;

    /* Get the number of arguments. */
    int iNumArgs = pStk->GetNumArgs();

    /* This will keep track of the this pointer if necessary. */
    PyObject* pThisPtr = NULL;
    int i = 0;

    /* Extra handling for thiscalls. */
    if( pFuncObj->GetConvention() == CONV_THISCALL )
    {
#ifdef __linux__
        /* On linux, the thisptr is actually passed on the stack as a parameter
        * if the function is a thiscall. If this is the case, skip the first
        * parameter because we want it at the end of the argument list that
        * is passed to the python function.
        */

        /* If we skip the first argument, we cannot read the last argument by
        * using our instance of CFuncStack.
        *
        * r_esp +  0: Return address
        * r_esp +  4: thisptr
        * r_esp +  8: Argument 1
        * r_esp + 12: Argument 2
        * ...
        */

        //i++;

        /* Parse out the this pointer as the first object. */
        void* pArg = ReadArg<void*>((void *)(pRegisters->r_esp + 4));
#else
        /* This is stored in ecx on windows. */
        void* pArg = (void *)(pRegisters->r_ecx);
#endif

        /* Build a value for the this pointer. */
        pThisPtr = Py_BuildValue("i", pArg);
    }

    for( ; i < iNumArgs; i++ )
    {
        /* Grab the argument struct. */
        ArgNode_t* pArgNode = pStk->GetArgument(i);
        CFuncArg* pArg = pArgNode->m_pArg;

        /* This will hold the final constructed python object. */
        PyObject* pVal = NULL;

        /* The address of the argument. */
        int offset = pArgNode->m_nOffset;

        // Hack for thiscalls on Linux
#ifdef __linux__
        if (pFuncObj->GetConvention() == CONV_THISCALL)
        offset += 4;
#endif

        DevMsg("Offset: %d\n", offset);

        /* Note, we add +4 to skip the return address. We don't need it here. */
        void* pArgAddr = (void *)(pRegisters->r_esp + offset + 4);

        /* Figure out what to do based on the argument type. */
        switch( pArg->GetType() )
        {
            /* Integer arguments. */
            case TYPE_INT32:
            case TYPE_INT32_PTR:
            case TYPE_BOOL:
                pVal = Py_BuildValue("i", ReadArg<long>(pArgAddr));
                break;
            case TYPE_FLOAT:
                pVal = Py_BuildValue("f", ReadArg<float>(pArgAddr));
                break;

            /* String args. */
            case TYPE_CHAR_PTR:
            {
                char* strarg = ReadArg<char*>(pArgAddr);
                DevMsg("Char arg: %s\n", strarg);
                pVal = Py_BuildValue("s", strarg);
                break;
            }

            default:
                DevMsg("[SPE] Argument #%d is not known! You're probably going to crash now!\n", i);
                break;
        }

        /* Add the argument to the list. */
        if( pVal ) {
            PyList_Append(m_pArgList, pVal);
            Py_XDECREF( pVal );
        }
    }

    /* Hack for thiscalls. */
    if( pThisPtr )
    {
        PyList_Append(m_pArgList, pThisPtr);
        Py_XDECREF(pThisPtr);
    }

    /* If we are here, we should be good. */
    m_bOK = true;
}

// =======================================================================
// Writes the values to the stack.
// =======================================================================
void CPythonStack::PutArgs( CDetour* pDetour )
{
    /* Make sure the input is valid. */
    if( !pDetour )
        return;

    /* Get the function object. */
    CFuncObj* Function = pDetour->GetFuncObj();

    /* Get registers. */
    CRegisterObj* Registers = pDetour->GetAsmBridge()->GetConv()->GetRegisters();

    // Define i outside of the loop. We use it later again
    int i = 0;
    for(i; i < Function->GetNumArgs(); i++ )
    {
        /* Get information about the argument. */
        ArgNode_t* pNode = Function->GetStack()->GetArgument(i);
        int StkOffset = pNode->m_nOffset;

        // Hack for thiscalls on Linux
#ifdef __linux__
        if (Function->GetConvention() == CONV_THISCALL)
            StkOffset += 4;
#endif

        eArgType ArgType = pNode->m_pArg->GetType();

        /* Get the python object representing the arg value. */
        DevMsg("Retrieving argument %d....\n", i);
        PyObject* PyArgVal = PyList_GetItem(m_pArgList, i);

        /* Make sure we're valid. */
        if( !PyArgVal )
        continue;

        /* Read it back onto the stack. */
        switch( ArgType )
        {
            /* Integer args. */
            case TYPE_BOOL:
            case TYPE_INT32:
            case TYPE_INT32_PTR:
                INT32 val;
                PyArg_Parse(PyArgVal, "i", &val);
                DevMsg("Argument %d is an integer arg with new value of %d\n", i, val);
                SetArg<INT32>((void *)(Registers->r_esp + StkOffset + 4), val);
                break;

            /* String args. */
            case TYPE_CHAR_PTR:
            char* strval = NULL;
                PyArg_Parse(PyArgVal, "s", &strval);
                DevMsg("Argument %d is a string with new value of %s\n", i, strval);
                SetArg<char*>((void *)(Registers->r_esp + StkOffset + 4), strval);
                break;
        }
    }

    // If it isn't a thiscall, we are finish here
    if( Function->GetConvention() != CONV_THISCALL )
        return;

    PyObject* pyThis = PyList_GetItem(m_pArgList, i);

    INT32 pThis = 0;
    PyArg_Parse(pyThis, "i", &pThis);

    DevMsg("This pointer has value of %d\n", pThis);

    /* Set the this pointer if it's valid. */
    if( pThis )
    {
#ifdef _WIN32
        /* This gets restored by the calling convention class. */
        Registers->r_ecx = pThis;
#else
        /* On linux, the this pointer is the last argument passed
        * onto the stack so we need to set it.
        */
        DevMsg("Setting the this pointer to %d\n", pThis);
        INT32* pStkAddr = (INT32 *)(Registers->r_esp + 4);
        *pStkAddr = pThis;
#endif
    }
}

// =======================================================================
// Stack constructor.
// =======================================================================
CPythonStack::CPythonStack( CDetour* pDetour )
{
    /* Set default. */
    m_bOK = false;

    /* Make sure input is valid. */
    if( !pDetour )
        return;

    /* Read args off the stack. */
    ReadArgs( pDetour );
}

// =======================================================================
// Destructor
// =======================================================================
CPythonStack::~CPythonStack( void )
{
    Py_XDECREF(m_pArgList);
}

// =======================================================================
// Adds a callback to the list.
// =======================================================================
void CPythonCallback::Add( void* pFuncObj, eHookType type )
{
    /* Make sure the input is valid. */
    if( !pFuncObj )
        return;

    /* Add it to the proper list. */
    switch( type )
    {
        case TYPE_PRE:
            m_PreCalls.AddToTail((PyObject *)pFuncObj);
            break;
        case TYPE_POST:
            m_PostCalls.AddToTail((PyObject *)pFuncObj);
            break;
    }
}

// =======================================================================
// Removes a callback from the list.
// =======================================================================
void CPythonCallback::Remove( void* pFuncObj, eHookType type )
{
    /* Make sure the input is valid. */
    if( !pFuncObj )
        return;

    /* This is the pointer to the vector we're going to search. */
    CUtlVector<PyObject*>* pCallbackList;

    /* Remove the callback from the proper list. */
    switch( type )
    {
        case TYPE_PRE:
            pCallbackList = &m_PreCalls;
            break;
        case TYPE_POST:
            pCallbackList = &m_PostCalls;
            break;
    }

    /* Find and remove the function. */
    pCallbackList->FindAndRemove((PyObject *)pFuncObj);
}

// =======================================================================
// Executes all of the precalls.
// =======================================================================
HookRetBuf_t* CPythonCallback::DoPreCalls( CDetour* pDet )
{
    /* Make sure that the input is valid. */
    if( !pDet )
        return NULL;

    /* This will store the highest priority return
     * action.
     */
    HookRetBuf_t* pBuf = new HookRetBuf_t;
    pBuf->eRes = HOOKRES_ERROR;
    pBuf->pRetBuf = NULL;

    /***********************************
     * Stage 1: Read the arguments off *
     *  of the stack.                  *
     ***********************************/
    DevMsg("Parsing arguments off of the stack.\n");
    CPythonStack pyArgStack( pDet );

    /* Make sure the arguments are OK. */
    if( !pyArgStack.IsOK() )
        return NULL;

    /***********************************
     * Stage 2: Execute the callbacks. *
     ***********************************/
    DevMsg("Executing pre-call functions.\n");

    for( int i = 0; i < m_PreCalls.Count(); i++ )
    {
        /* Get the python object. */
        PyObject* pFuncObj = m_PreCalls[i];

        /* Call the function and get the return value. */
        DevMsg("Executing function %d\n", i);
        PyObject* pRetTup = PyObject_CallFunction(pFuncObj, "O", pyArgStack.GetArgs());

        if( PyErr_Occurred() || !pRetTup ) {
            PyErr_Print();
            continue;
        }

        /* Parse out the return tuple:
        *  1st element: The action to take.
        *  2nd element: The return value.
        */
        eHookRes hookAction = HOOKRES_ERROR;
        void*    retval = NULL;
        if( !PyArg_ParseTuple(pRetTup, "ii", &hookAction, &retval) )
        {
            /* TODO: Some kind of logging. */
            DevMsg("[SPE] Could not callback return tuple!\n");
            continue;
        }

        /* Decrement the reference. */
        DevMsg("Decref'ing the return tuple.\n");
        Py_XDECREF(pRetTup);

        /* Bubble up actions that have higher priority. */
        DevMsg("Performing hookaction logic\n");

        if( hookAction >= pBuf->eRes )
        {
            pBuf->eRes = hookAction;
            pBuf->pRetBuf = retval;
        }

        DevMsg("Done\n");
    }

    /***********************************
    * Stage 3: Read args back onto the*
    *  stack.                          *
    ***********************************/
    if( pBuf->eRes == HOOKRES_NEWPARAMS ) {
        DevMsg("Placing the arguments back onto the stack.\n");
        pyArgStack.PutArgs( pDet );
    }

    /***********************************
    * Stage 4: Pass return value back *
    *  to the library handler.         *
    ***********************************/
    return pBuf;
}

// =======================================================================
// Executes all of the post calls.
// =======================================================================
HookRetBuf_t* CPythonCallback::DoPostCalls( CDetour* pDet )
{
    /* Make sure that the input is valid. */
    if( !pDet )
        return NULL;

    /* This will store the highest priority return
     * action.
     */
    HookRetBuf_t* pBuf = new HookRetBuf_t;

    /***********************************
     * Stage 1: Read the arguments off *
     *  of the stack.                  *
     ***********************************/
    CPythonStack pyArgStack( pDet );

    /* Make sure the arguments are OK. */
    if( !pyArgStack.IsOK() )
        return NULL;

    /***********************************
     * Stage 2: Execute the callbacks. *
     ***********************************/
    for( int i = 0; i < m_PostCalls.Count(); i++ )
    {
        /* Get the python object. */
        PyObject* pFuncObj = m_PostCalls[i];

        /* Call the function and get the return value. */
        PyObject* pRetTup = PyObject_CallFunction(pFuncObj, "(O)",
            pyArgStack.GetArgs() );

        /* Make sure we got a valid result back. */
        if( !pRetTup ) {
            Msg("NULL return value from callfunction!\n");
            continue;
        }

        /* Parse out the return tuple:
         *  1st element: The action to take.
         *  2nd element: The return value.
         */
        eHookRes hookAction;
        void*    retval;
        if( !PyArg_ParseTuple(pRetTup, "ii", &hookAction, &retval) )
        {
            /* TODO: Some kind of logging. */
            continue;
        }

        /* Decrement the reference. */
        Py_XDECREF(pRetTup);

        /* Bubble up actions that have higher priority. */
        if( hookAction >= pBuf->eRes )
        {
            pBuf->eRes = hookAction;
            pBuf->pRetBuf = retval;
        }
    }

    /***********************************
     * Stage 4: Pass return value back *
     *  to the library handler.        *
     ***********************************/
    return pBuf;
}
