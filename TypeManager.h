#pragma once
struct AstNode;

static const uint32_t CASTFLAG_NOERROR = 0x00000001;
static const uint32_t CASTFLAG_DBLSIDE = 0x00000002;

struct TypeManager
{
    static bool castError(SourceFile* sourceFile, TypeInfo* requestedType, AstNode* nodeToCast, uint32_t castFlags);

    static bool castToNativeBool(SourceFile* sourceFile, AstNode* nodeToCast, uint32_t castFlags);
    static bool castToNativeU8(SourceFile* sourceFile, AstNode* nodeToCast, uint32_t castFlags);
    static bool castToNativeU16(SourceFile* sourceFile, AstNode* nodeToCast, uint32_t castFlags);
    static bool castToNativeU32(SourceFile* sourceFile, AstNode* nodeToCast, uint32_t castFlags);
    static bool castToNativeU64(SourceFile* sourceFile, AstNode* nodeToCast, uint32_t castFlags);
    static bool castToNativeS8(SourceFile* sourceFile, AstNode* nodeToCast, uint32_t castFlags);
    static bool castToNativeS16(SourceFile* sourceFile, AstNode* nodeToCast, uint32_t castFlags);
    static bool castToNativeS32(SourceFile* sourceFile, AstNode* nodeToCast, uint32_t castFlags);
    static bool castToNativeS64(SourceFile* sourceFile, AstNode* nodeToCast, uint32_t castFlags);
    static bool castToNativeF32(SourceFile* sourceFile, AstNode* nodeToCast, uint32_t castFlags);
    static bool castToNativeF64(SourceFile* sourceFile, AstNode* nodeToCast, uint32_t castFlags);

    static bool castToNative(SourceFile* sourceFile, TypeInfo* requestedType, AstNode* nodeToCast, uint32_t castFlags);

    static bool makeCompatibles(SourceFile* sourceFile, TypeInfo* requestedType, AstNode* nodeToCast, uint32_t castFlags = 0);

    static string nativeTypeName(TypeInfo* type);
};
