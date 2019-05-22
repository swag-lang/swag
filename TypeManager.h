#pragma once
struct AstNode;

enum class castResult
{
    Ok,
    FailWithoutError,
    FailWithError,
};

struct TypeManager
{
    static castResult castToNativeU8(SourceFile* sourceFile, AstNode* fromNode, uint32_t castFlags, TypeInfo** result);
	static castResult castToNativeU16(SourceFile* sourceFile, AstNode* fromNode, uint32_t castFlags, TypeInfo** result);
	static castResult castToNativeU32(SourceFile* sourceFile, AstNode* fromNode, uint32_t castFlags, TypeInfo** result);
	static castResult castToNativeU64(SourceFile* sourceFile, AstNode* fromNode, uint32_t castFlags, TypeInfo** result);
    static castResult castToNative(SourceFile* sourceFile, AstNode* requestedTypeNode, AstNode* fromNode, uint32_t castFlags, TypeInfo** result);
    static TypeInfo*  makeCompatibles(SourceFile* sourceFile, AstNode* sourceNode, AstNode* fromNode, uint32_t castFlags = 0);
};
