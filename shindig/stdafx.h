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
#include <boost/scoped_ptr.hpp>

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
#include <celsus/file_utils.hpp>
#include <celsus/vertex_types.hpp>
#include <celsus/refptr.hpp>
#include <celsus/text_scanner.hpp>
#include <celsus/math_utils.hpp>
#include <celsus/file_utils.hpp>
#include <celsus/file_watcher.hpp>
#include <celsus/UnicodeUtils.hpp>
#include <celsus/fast_delegate.hpp>
#include <celsus/fast_delegate_bind.hpp>
#include <celsus/dynamic_vb.hpp>
#include <celsus/vertex_types.hpp>

#include <libs/json_spirit/json_spirit.h>

#include <D3DX10math.h>
#include <hash_map>

#include <agents.h>

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
};

#include <shobjidl.h>
#include <Shlguid.h>
#include <Shlobj.h>
#include <direct.h>

#include <WS2tcpip.h>
