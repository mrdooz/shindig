// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

// A macro to disallow the copy constructor and operator= functions
// This should be used in the private: declarations for a class
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
	TypeName(const TypeName&);               \
	void operator=(const TypeName&)

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <vector>
#include <map>
#include <string>

#include <D3DX11.h>
#include <D3Dcompiler.h>
#include <cassert>
#include <crtdefs.h>
#include <stdint.h>
#include <atlbase.h>
#include <fstream>

#include <boost/scoped_array.hpp>
#include <boost/signals2.hpp>
//#include <boost/filesystem.hpp>

#include <celsus/celsus.hpp>
#include <celsus/logger.hpp>
#include <Celsus/DX11Utils.hpp>
#include <celsus/CelsusExtra.hpp>
#include <celsus/ErrorHandling.hpp>
#include <celsus/ChunkIO.hpp>
#include <celsus/CelsusExtra.hpp>
#include <celsus/path_utils.hpp>
#include <celsus/string_utils.hpp>
#include <celsus/error2.hpp>
#include <celsus/graphics.hpp>
#include <celsus/effect_wrapper.hpp>
#include <celsus/section_reader.hpp>
#include <celsus/D3D11Descriptions.hpp>

#include "fast_delegate.hpp"
#include "fast_delegate_bind.hpp"

#include <libs/json_spirit/json_spirit.h>

#include <D3DX10math.h>
#include <hash_map>

#include <agents.h>
