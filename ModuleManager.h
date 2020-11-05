#pragma once
#include "Utf8.h"

struct DataSegment;
struct AstFuncDecl;

struct ByteCodeRunContext;

struct ModuleManager
{
    bool  loadModule(const Utf8& moduleName, bool canBeSystem = true);
    bool  isModuleLoaded(const Utf8& moduleName);
    bool  isModuleFailedLoaded(const Utf8& moduleName);
    void* getFnPointer(const Utf8& moduleName, const Utf8& funcName);
    void  addPatchFuncAddress(void** patchAddress, AstFuncDecl* func);
    void  applyPatches(const Utf8& moduleName, void* moduleHandle);

    shared_mutex     mutex;
    shared_mutex     mutexLoaded;
    map<Utf8, void*> loadedModules;
    set<Utf8>        failedLoadedModules;

    struct PatchOffset
    {
        void**       patchAddress;
        AstFuncDecl* funcDecl;
    };

    shared_mutex                   mutexPatch;
    map<Utf8, vector<PatchOffset>> patchOffsets;
};

extern ModuleManager g_ModuleMgr;