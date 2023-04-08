#pragma once

#ifdef __clang__

#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Wreorder-ctor"
#pragma clang diagnostic ignored "-Wtypename-missing"
#pragma clang diagnostic ignored "-Wsign-compare"
#pragma clang diagnostic ignored "-Wmicrosoft-include"
#pragma clang diagnostic ignored "-Wswitch"
#pragma clang diagnostic ignored "-Wmissing-field-initializers"
#pragma clang diagnostic ignored "-Wunqualified-std-cast-call"
#pragma clang diagnostic ignored "-Wmissing-braces"
#pragma clang diagnostic ignored "-Wdelete-non-abstract-non-virtual-dtor"
#pragma clang diagnostic ignored "-Wnonportable-include-path"
#pragma clang diagnostic ignored "-Wmicrosoft-cast"
#pragma clang diagnostic ignored "-Wchar-subscripts"
#pragma clang diagnostic ignored "-Wlogical-op-parentheses"
#pragma clang diagnostic ignored "-Wbitwise-op-parentheses"
#define SWAG_FORCE_INLINE __attribute__((always_inline))
#define SWAG_UNREACHABLE __builtin_unreachable()

#else

#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#define _SILENCE_CXX20_CISO646_REMOVED_WARNING
#define SWAG_FORCE_INLINE __forceinline
#define SWAG_UNREACHABLE __assume(false)

#endif

////////////////////////////
#include "LLVMInc.h"

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