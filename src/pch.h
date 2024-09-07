// ReSharper disable CppInconsistentNaming
#pragma once

#ifdef __clang__

#pragma clang diagnostic ignored "-Wmicrosoft-include"
#pragma clang diagnostic ignored "-Wnonportable-include-path"
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Wswitch"
#pragma clang diagnostic ignored "-Wvarargs"
#define SWAG_FORCE_INLINE __attribute__((always_inline))
#define SWAG_UNREACHABLE  __builtin_unreachable()

#else

#define _SILENCE_CXX20_CISO646_REMOVED_WARNING
#define SWAG_FORCE_INLINE __forceinline
#define SWAG_UNREACHABLE  __assume(false)

#endif

////////////////////////////
#include "Backend/LLVM/LLVM_Inc.h"

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

////////////////////////////

#ifdef SWAG_DEV_MODE
#define SWAG_HAS_MEMORY_CHECK
#define SWAG_HAS_ASSERT
#define SWAG_HAS_RACE_CONDITION
#define SWAG_HAS_RACE_CONDITION_VECTOR
#define SWAG_HAS_RACE_CONDITION_SEGMENT
#endif

#define SWAG_HAS_RACE_CONDITION_VECTOR
#define SWAG_HAS_RACE_CONDITION_SEGMENT

////////////////////////////

#include "Threading/RaceCondition.h"
#include "Core/Allocator.h"
#include "Core/Map.h"
#include "Core/Set.h"
#include "Core/Utf8.h"
#include "Core/Vector.h"
#include "Core/VectorNative.h"
#include "Threading/RaceCondition.h"
