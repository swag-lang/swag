#pragma once

#ifdef __clang__

#pragma clang diagnostic ignored "-Wmicrosoft-include"
#pragma clang diagnostic ignored "-Wnonportable-include-path"
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Wreorder-ctor"
#define SWAG_FORCE_INLINE __attribute__((always_inline))
#define SWAG_UNREACHABLE __builtin_unreachable()

#else

#define _SILENCE_CXX20_CISO646_REMOVED_WARNING
#define SWAG_FORCE_INLINE __forceinline
#define SWAG_UNREACHABLE __assume(false)

#endif

////////////////////////////
#include "LLVM_Inc.h"

////////////////////////////

#include <vector>
#include <deque>
#include <map>
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

#include "Allocator.h"
#include "Vector.h"
#include "VectorNative.h"
#include "Utf8.h"
#include "Map.h"
#include "Set.h"