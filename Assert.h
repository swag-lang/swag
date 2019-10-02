#pragma once
struct SourceFile;
struct AstNode;

#ifdef SWAG_HAS_ASSERT
struct DiagnosticInfos
{
    string      pass;
    SourceFile* sourceFile = nullptr;
    AstNode*    node       = nullptr;

    void clear()
    {
        pass.clear();
        sourceFile = nullptr;
        node       = nullptr;
    }
};

extern thread_local DiagnosticInfos g_diagnosticInfos;
extern void                         swag_assert(const char* expr, const char* file, int line);

#define SWAG_ASSERT(__expr)                           \
    {                                                 \
        if (!(__expr))                                \
        {                                             \
            swag_assert(#__expr, __FILE__, __LINE__); \
        }                                             \
    }

#else
#define SWAG_ASSERT(__expr) {}
#endif