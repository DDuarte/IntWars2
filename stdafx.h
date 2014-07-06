/*
IntWars playground server for League of Legends protocol testing
Copyright (C) 2012  Intline9 <Intline9@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef _STDAFX_H
#define _STDAFX_H
// Windows
#include <winsock2.h>
#include <windows.h>
#include <windowsx.h>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "User32.lib")
#pragma comment(lib, "advapi32.lib")
// Directx
#include <d3d9.h>
#include <d3dx9.h>
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")
// Standard library
#include <stdio.h>
#include <cstring>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <list>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <memory>
#include <ctime>
#include <chrono>
#define _USE_MATH_DEFINES
#include <math.h>
// Lua
extern "C" {
#include "LUA/lua.h"
#include "LUA/lualib.h"
#include "LUA/lauxlib.h"
}
#include <luabind/luabind.hpp>
// In-Project Libraries
#include "..\vmath.h"
#include "..\Log.h"

#define SAFE_RELEASE(p) { if ( (p) ) { (p)->Release(); (p) = 0; } }
#define SAFE_DELETE(a) { if( (a) ) { delete (a); (a) = NULL; } }

typedef unsigned char byte;
typedef char int8;
typedef short int16;
typedef int int32;
typedef long long int64;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;


#define PKT_MAX = 0x100
#define Logging Log::getMainInstance()

#endif