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

#include <assert.h>
#include <atomic>
#include <codecvt>
#include <comdef.h>
#include <deque>
#include <fcntl.h>
#include <filesystem>
#include <fstream>
#include <functional>
#include <io.h>
#include <iostream>
#include <locale>
#include <map>
#include <mutex>
#include <ranges>
#include <set>
#include <shared_mutex>
#include <source_location>
#include <string>
#include <unordered_set>
#include <vector>

using namespace std;

////////////////////////////

#ifdef SWAG_DEV_MODE
#define SWAG_CHECK_MEMORY
#define SWAG_HAS_ASSERT
#endif

////////////////////////////

#include "Allocator.h"
#include "Map.h"
#include "Set.h"
#include "Utf8.h"
#include "Vector.h"
#include "VectorNative.h"
