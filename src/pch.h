#pragma once

#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#define _SILENCE_CXX20_CISO646_REMOVED_WARNING

////////////////////////////
#include "LLVMInc.h"

////////////////////////////

#include <Vector>
#include <deque>
#include <Map>
#include <assert.h>
#include <mutex>
#include <iostream>
#include <atomic>
#include <locale>
#include <codecvt>
#include <set>
#include <fstream>
#include <functional>
#include <string>
#include <shared_mutex>
#include <unordered_set>
#include <fcntl.h>
#include <io.h>
#include <comdef.h>
#include <source_location>
#include <filesystem>

using namespace std;

////////////////////////////

#ifdef SWAG_DEV_MODE
#define SWAG_CHECK_MEMORY
#define SWAG_HAS_ASSERT
#endif

////////////////////////////

#ifdef _MSC_VER
#define SWAG_FORCE_INLINE __forceinline
#define SWAG_UNREACHABLE __assume(false)
#endif
