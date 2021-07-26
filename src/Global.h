#pragma once
#include "Utf8.h"
#include "Assert.h"

struct Global
{
    void        setup();
    atomic<int> uniqueID        = 0;
    atomic<int> compilerAllocTh = 0;
    bool        exiting         = false;
};

extern void* doForeignLambda(void* ptr);
extern bool  isForeignLambda(void* ptr);
extern void* undoForeignLambda(void* ptr);
extern void* doByteCodeLambda(void* ptr);
extern void* undoByteCodeLambda(void* ptr);
extern bool  isByteCodeLambda(void* ptr);

extern struct Global g_Global;