// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>

#include <string>
#include <vector>
#include <tchar.h>
#include <comdef.h>

#include "lib/aimp_sdk/apiPlugin.h"
#include "lib/aimp_sdk/apiCore.h"
#include "lib/aimp_sdk/apiObjects.h"
#include "lib/aimp_sdk/apiMUI.h"
#include "lib/aimp_sdk/apiMusicLibrary.h"

#include "IUnknownInterfaceImpl.h"
#include "AimpString.h"

#include "aimp_dlna.h"