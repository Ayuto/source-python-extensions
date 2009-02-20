//=============================================================================//
// Copyright: Deniz Sezen
//		Defines this modules python methods
//=============================================================================//

#include "esx_player_manager.h"
#include "esx_py_methods.h"
#include "esx_signature_manager.h"
#include "esx_dyncall_py.h"

/* A new call VM */
DCCallVM* vm = dcNewCallVM( 4096 );
bool bThisCallActive = false;

//=============================================================================
// >> Returns a CObject to a userid.
//=============================================================================
PyObject* esx_GetPlayerPtr( PyObject* self, PyObject* args )
{
	int userid = 0;

	if( !PyArg_ParseTuple( args, "i", &userid ) )
	{
		Msg("[ESX] Could not parse tuple in GetPlayerPtr!\n");
		return Py_BuildValue("");
	}

	void* ent = gPlayerManager->GetPlayerByUserID(userid)->GetUnknown()->GetBaseEntity();
	Msg("[ESX] Player pointer: %i\n", ent);
	return PyCObject_FromVoidPtr( ent, NULL );
}

//=============================================================================
// >> Returns a CObject to a function, using a signature
//=============================================================================
PyObject* esx_FindFuncPtr( PyObject* self, PyObject* args )
{
	PyObject* p;
	int length = 0;

	if( !PyArg_ParseTuple( args, "Si", &p, &length ) )
	{
		Msg("[ESX] Could not parse tuple in FindFuncPtr\n");
		return PyCObject_FromVoidPtr( NULL, NULL );
	}

	void* addr = gSigger->findFunction( PyString_AsString(p), length );
	Msg("[ESX] Function ptr: %i\n", addr);
	
	if( !addr )
		return Py_BuildValue("");
	
	return PyCObject_FromVoidPtr( addr , NULL );
}

//=============================================================================
// >> Sets the calling convention of the virtual machine
//=============================================================================
PyObject* esx_SetCallingConvention( PyObject* self, PyObject* args )
{
	const char* conv;

	if( !PyArg_ParseTuple( args, "s", &conv ) )
	{
		Msg("[ESX] Could not parse tuple in SetCallingConvention!\n");
		return Py_BuildValue("");
	}

	dcReset( vm );

	if( strcmp( conv, "thiscall" ) == 0 )
	{
		#ifdef _WIN32
			dcMode( vm, DC_CALL_C_X86_WIN32_THIS_MS );
		#else
			dcMode( vm, DC_CALL_C_X86_WIN32_THIS_GNU );
		#endif
		
		bThisCallActive = true;

		return Py_BuildValue("");
	}

	else if( strcmp( conv, "fastcall" ) == 0 )
	{
		#ifdef _WIN32
			dcMode( vm, DC_CALL_C_X86_WIN32_FAST_MS );
		#else
			dcMode( vm, DC_CALL_C_X86_WIN32_FAST_GNU);
		#endif

		return Py_BuildValue("");
	}

	else if( strcmp( conv, "cdecl" ) == 0 )
	{
		dcMode( vm, DC_CALL_C_DEFAULT );
		return Py_BuildValue("");
	}

	else
	{
		Msg("[ESX] Unknown calling convention type!\n");
		return Py_BuildValue("");
	}

	return Py_BuildValue("");
}
//=============================================================================
// >> Calls a function through the use of dyncall
//=============================================================================
/* Calls a function through the use of dyncall */
PyObject* esx_CallFunction( PyObject* self, PyObject* args )
{
	PyObject* func_ptr = NULL;
	PyObject* func_args = NULL;
	const char* signature = NULL;
	const char* ptr;
	int size;
	int pos = 0;
	char ch;
	void* function_pointer = NULL;

	if( !PyArg_ParseTuple( args, "OsO", &func_ptr, &signature, &func_args ) )
	{
		Msg("[ESX] Error parsing out function args, signature, and pointer!\n");
		return Py_BuildValue("");
	}

	/* Parse out the function pointer */
	function_pointer = PyCObject_AsVoidPtr( func_ptr );

	/* Make sure it's valid */
	if( !function_pointer )
	{
		Msg("[ESX] The function pointer is NULL!\n");
		return Py_BuildValue("");
	}

	/* Make sure to flush the VM */
	dcReset( vm );

	/* Get the size of the argument list */
	size = PyTuple_Size( func_args );

	ptr = signature;

	/* Start going through the arguments */
	while ( (ch = *ptr) != '\0' && ch != ')' ) 
	{
		/* Create a pyobject to hold the argument */
		PyObject* arg;

		//Increment the index
		//int index = pos + 1;

		arg = PyTuple_GetItem( func_args, pos );

		/* Do some comparisons */
		switch( ch )
		{			
		case DC_SIGCHAR_BOOL:
			DCbool b;
			PyArg_ParseTuple( arg, "i", &b );
			dcArgBool( vm, b );
			break;

		case DC_SIGCHAR_CHAR:
			{
				DCchar c;
				size_t l;
				char* s;
				l = PyString_GET_SIZE(arg);

				if (l != 1)
				{
					Msg("[ESX] String mismatch. Expected a string!");
					return Py_BuildValue("");
				}

				s = PyString_AsString(arg);          
				c = (DCchar) s[0];
				dcArgChar( vm, c );
				break;
			}

		case DC_SIGCHAR_SHORT:
			{
				DCshort s;
				long v;
				v = PyInt_AS_LONG(arg);

				if ( (v < SHRT_MIN) || (v > SHRT_MAX) )
				{
					Msg( "[ESX] CallFunction: value out of range at argument %d - expecting a short value\n" );
					return Py_BuildValue("");
				}

				s = (DCshort) v;
				dcArgShort( vm, s);
				break;
			} 

		case DC_SIGCHAR_INT:
			{
				int v;
				v = PyInt_AsLong( arg );
				dcArgInt( vm, (DCint) v );
				break;
			}

		case DC_SIGCHAR_FLOAT:
			{
				DCfloat f;
				f = (float) PyFloat_AsDouble( arg );
				dcArgFloat( vm, f );
				break;
			}

		case DC_SIGCHAR_POINTER:
			{
				DCpointer ptr;
				ptr = PyCObject_AsVoidPtr( arg );
				dcArgPointer( vm, ptr );
				break;
			}

		case 'S':
			{
				char* p;
				p = PyString_AsString(arg);
				dcArgPointer( vm, (DCpointer) p );
				break;
			}

		default:
			Msg("[ESX] Unknown char signature!\n");
			return Py_BuildValue("");
			break;
		}

		++pos; ++ptr;

	} //End while

	if (pos != size)
	{
		Msg("[ESX] pos != size!\n");
		return Py_BuildValue("");
	}

	if (ch == '\0')
	{
		Msg("[ESX] ch == null terminated\n");
		return Py_BuildValue("");
	}

	ch = *++ptr;

	PyObject* p;
	switch(ch) 
	{
	case DC_SIGCHAR_VOID: dcCallVoid( vm, function_pointer ); p = Py_BuildValue(""); break;
	case DC_SIGCHAR_BOOL: p = Py_BuildValue("i", dcCallBool( vm, function_pointer ) ); break;
	case DC_SIGCHAR_INT: p = Py_BuildValue("i", dcCallInt( vm, function_pointer ) ); break;
	case DC_SIGCHAR_LONGLONG: p = Py_BuildValue("L", (unsigned long long) dcCallLongLong( vm, function_pointer ) ); break;
	case DC_SIGCHAR_FLOAT: p = Py_BuildValue("f", dcCallFloat( vm, function_pointer ) ); break; 
	case DC_SIGCHAR_DOUBLE: p = Py_BuildValue("d", dcCallDouble( vm, function_pointer ) ); break;
	case 's': p = Py_BuildValue("s", dcCallPointer( vm, function_pointer ) ); break;
	case DC_SIGCHAR_POINTER: p = Py_BuildValue("p", dcCallPointer( vm, function_pointer ) ); break;
	default:  Msg("[ESX] Invalid p = type signature.\n" ); p = Py_BuildValue(""); break;
	}

	return p;
}