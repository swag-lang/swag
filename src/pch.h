#pragma once

#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING

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

using namespace std;
namespace fs = std::experimental::filesystem;

////////////////////////////

#define FFI_BUILDING
#ifdef SWAG_DEV_MODE
#define SWAG_CHECK_MEMORY
#define SWAG_HAS_ASSERT
#endif

////////////////////////////
