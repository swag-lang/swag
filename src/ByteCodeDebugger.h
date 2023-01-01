#pragma once
struct ByteCodeRunContext;
struct TypeInfo;
struct ComputedValue;
struct Utf8;
enum class CompilerAstKind;

struct ByteCodeDebugger
{
    struct EvaluateResult
    {
        TypeInfo*      type  = nullptr;
        void*          addr  = nullptr;
        ComputedValue* value = nullptr;
        void*          storage[2];
    };

    struct ValueFormat
    {
        int  bitCount = 8;
        bool isSigned = false;
        bool isFloat  = false;
        bool isHexa   = true;
        bool print0x  = true;
    };

    static bool evalDynExpression(ByteCodeRunContext* context, const Utf8& expr, EvaluateResult& res, CompilerAstKind kind, bool silent = false);
    static bool evalExpression(ByteCodeRunContext* context, const Utf8& expr, EvaluateResult& res, bool silent = false);

    static bool getValueFormat(const Utf8& cmd, ValueFormat& fmt);

    static void appendTypedValue(ByteCodeRunContext* context, Utf8& str, const EvaluateResult& res, int indent, ValueFormat* fmt = nullptr);
    static void appendTypedValueProtected(ByteCodeRunContext* context, Utf8& str, const EvaluateResult& res, int indent, ValueFormat* fmt = nullptr);
    static void appendLiteralValue(ByteCodeRunContext* context, Utf8& result, const ValueFormat& fmt, const void* addr);
    static void appendLiteralValueProtected(ByteCodeRunContext* context, Utf8& result, const ValueFormat& fmt, const void* addr);

    static void printMemory(ByteCodeRunContext* context, const Utf8& arg);

    static bool step(ByteCodeRunContext* context);

    static void printHelp();
};
