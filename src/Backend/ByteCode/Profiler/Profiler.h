#pragma once
#ifdef SWAG_STATS
struct AstFuncDecl;

struct FFIStat
{
    Utf8     name;
    uint32_t count;
    uint64_t cum;
};

extern void profiler();
#endif
