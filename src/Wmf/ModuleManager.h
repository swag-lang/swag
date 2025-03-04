#pragma once
#include "Threading/Mutex.h"

struct AstFuncDecl;
struct ByteCodeRunContext;
struct ModuleDependency;
struct DataSegment;

struct ModuleManager
{
    void  resetFailedModule(const Utf8& moduleName);
    void  addPatchFuncAddress(DataSegment* seg, void** patchAddress, AstFuncDecl* func);
    bool  loadModule(const Utf8& name, bool canBeSystem = true);
    bool  isModuleLoaded(const Utf8& moduleName);
    bool  isModuleFailedLoaded(const Utf8& moduleName);
    void* getFnPointer(const Utf8& moduleName, const Utf8& funcName);
    bool  applyPatches(const Utf8& moduleName, void* moduleHandle);

    static const Utf8& getForeignModuleName(const AstFuncDecl* func);

    SharedMutex    mutex;
    SharedMutex    mutexLoaded;
    MapUtf8<void*> loadedModules;
    SetUtf8        failedLoadedModules;

    struct PatchOffset
    {
        DataSegment* segment;
        void**       patchAddress;
        AstFuncDecl* funcDecl;
    };

    SharedMutex                  mutexPatch;
    MapUtf8<Vector<PatchOffset>> patchOffsets;
    Utf8                         loadModuleError;
};

extern ModuleManager* g_ModuleMgr;
#ifdef SWAG_DEV_MODE
extern void* g_SwagPatchMarker;
#endif
