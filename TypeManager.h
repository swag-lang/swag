#pragma once
struct AstNode;

enum class castResult
{
    Ok,
    FailWithoutError,
    FailWithError,
};

static const uint32_t CASTFLAG_NOERROR = 0x00000001;
static const uint32_t CASTFLAG_DBLSIDE = 0x00000002;

struct TypeManager
{
    static castResult castToNativeBool(SourceFile* sourceFile, AstNode* nodeToCast, uint32_t castFlags);
    static castResult castToNativeU8(SourceFile* sourceFile, AstNode* nodeToCast, uint32_t castFlags);
    static castResult castToNativeU16(SourceFile* sourceFile, AstNode* nodeToCast, uint32_t castFlags);
    static castResult castToNativeU32(SourceFile* sourceFile, AstNode* nodeToCast, uint32_t castFlags);
    static castResult castToNativeU64(SourceFile* sourceFile, AstNode* nodeToCast, uint32_t castFlags);
    static castResult castToNativeS8(SourceFile* sourceFile, AstNode* nodeToCast, uint32_t castFlags);
    static castResult castToNativeS16(SourceFile* sourceFile, AstNode* nodeToCast, uint32_t castFlags);
    static castResult castToNativeS32(SourceFile* sourceFile, AstNode* nodeToCast, uint32_t castFlags);
    static castResult castToNativeS64(SourceFile* sourceFile, AstNode* nodeToCast, uint32_t castFlags);
    static castResult castToNativeF32(SourceFile* sourceFile, AstNode* nodeToCast, uint32_t castFlags);
    static castResult castToNativeF64(SourceFile* sourceFile, AstNode* nodeToCast, uint32_t castFlags);

    static castResult castToNative(SourceFile* sourceFile, TypeInfo* requestedType, AstNode* nodeToCast, uint32_t castFlags);

    static bool makeCompatibles(SourceFile* sourceFile, TypeInfo* requestedType, AstNode* nodeToCast, uint32_t castFlags = 0);
    static bool makeCompatibles(SourceFile* sourceFile, AstNode* requestedTypeNode, AstNode* nodeToCast, uint32_t castFlags = 0);

    static string nativeTypeName(TypeInfo* type);
};
