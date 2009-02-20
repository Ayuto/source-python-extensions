//=============================================================================//
// Copyright: Deniz Sezen
//		Defines this modules python methods
//=============================================================================//

#ifndef ESX_SIGNATURE_MANAGER_H
#define ESX_SIGNATURE_MANAGER_H

#include "esx_scanner.h"

//=============================================================================
// >> Signature Scanner class
//=============================================================================
class CSigger
{
	private:
		void* m_addr;
	public:
		CSigger( void* addr );
		void* findFunction( const char* sig, int length );
};

extern CSigger* gSigger;

#endif

