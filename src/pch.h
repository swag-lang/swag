#pragma once

#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING

#ifdef __clang__
#pragma clang diagnostic ignored "-Wreorder-ctor"
#pragma clang diagnostic ignored "-Wswitch"
#pragma clang diagnostic ignored "-Wdelete-non-abstract-non-virtual-dtor"
#pragma clang diagnostic ignored "-Wsign-compare"
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Wunused-variable"
#pragma clang diagnostic ignored "-Wmicrosoft-include"
#pragma clang diagnostic ignored "-Wmissing-braces"
#pragma clang diagnostic ignored "-Wmicrosoft-cast"
#pragma clang diagnostic ignored "-Wchar-subscripts"
#pragma clang diagnostic ignored "-Wunused-const-variable"
#pragma clang diagnostic ignored "-Wimplicit-exception-spec-mismatch"
#pragma clang diagnostic ignored "-Wmissing-field-initializers"
#endif

////////////////////////////
#include "LLVMInc.h"

////////////////////////////

#include <vector>
#include <deque>
#include <map>
#include <assert.h>
#include <experimental/filesystem>
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

using namespace std;
namespace fs = std::experimental::filesystem;

////////////////////////////

#define FFI_BUILDING
#ifdef SWAG_DEV_MODE
#define SWAG_CHECK_MEMORY
#define SWAG_HAS_ASSERT
#endif

////////////////////////////
