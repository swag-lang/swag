#ifndef PCH_H
#define PCH_H

#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#include "windows.h"
#include <vector>
#include <map>
#include <assert.h>
#include <experimental/filesystem>
#include <mutex>
#include <iostream>
#include <atomic>
#include <locale>
#include <codecvt>

using namespace std;
namespace fs = std::experimental::filesystem;
#pragma warning(disable: 4100)

//#define SWAG_TEST_CPP

#endif
