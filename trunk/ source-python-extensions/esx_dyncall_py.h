//=============================================================================//
// Copyright: Deniz Sezen
//=============================================================================//

#ifndef ESX_DYNCALL_PY_H
#define ESX_DYNCALL_PY_H

#ifdef _WIN32
#pragma once
#endif

/* Includes */
#ifdef _WIN32
#include "Python.h"
#else
#include "Python25/Include/Python.h"
#endif
#include "dyncall/dyncall/dyncall.h"
#include "dyncall/dyncall/dyncall_signature.h"

#endif
