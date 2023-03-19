#pragma once
#ifdef SWAG_STATS
struct AstFuncDecl;

struct FFIStat
{
    AstFuncDecl* func;
    uint32_t     count;
    uint64_t     cum;
};

extern void profiler();
#endif
