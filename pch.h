#ifndef PCH_H
#define PCH_H

// LLVM stuff
// Before the rest, or it does not compile (yeahhh)
#pragma warning(push, 0)
#pragma warning(disable : 4996)
#pragma warning(disable : 4244)
#pragma warning(disable : 4702)
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/DIBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Transforms/IPO.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include "llvm/Transforms/Utils.h"
#include "llvm/Analysis/TargetLibraryInfo.h"

#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include "windows.h"
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
#pragma warning(pop)

using namespace std;
namespace fs = std::experimental::filesystem;
#pragma warning(disable : 4100)

extern void* Memcpy(void* destination, const void* source, size_t size);

#ifndef SWAG_IS_FINAL
#define SWAG_HAS_ASSERT
#define SWAG_HAS_PROFILE
#endif

#define FFI_BUILDING

#endif
