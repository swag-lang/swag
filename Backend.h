#pragma once
#include "OutputFile.h"
struct Module;
struct BuildParameters;
struct TypeInfoFuncAttr;
struct TypeInfoStruct;
struct TypeInfoEnum;
struct AstFuncDecl;
struct AstStruct;
struct AstNode;
struct Scope;
struct TypeInfoParam;
struct AstVarDecl;

struct Backend
{
    Backend(Module* mdl)
        : module{mdl}
    {
    }

    void generateSwg();
    void emitSeparator(Concat& buffer, const char* title);

    virtual bool preCompile();
    virtual bool compile(const BuildParameters& backendParameters) = 0;

    Module*    module;
    OutputFile bufferSwg;

    bool emitAttributes(AstNode* node);
    bool emitAttributes(TypeInfoParam* param);
    bool emitGenericParameters(AstNode* node);
    bool emitPublicEnumSwg(TypeInfoEnum* typeEnum, AstNode* node);
    bool emitPublicStructSwg(TypeInfoStruct* typeStruct, AstStruct* node);
    bool emitPublicConstSwg(AstVarDecl* node);
	bool emitPublicTypeAliasSwg(AstNode* node);
    bool emitPublicFuncSwg(TypeInfoFuncAttr* typeFunc, AstFuncDecl* node);
    bool emitFuncSignatureSwg(TypeInfoFuncAttr* typeFunc, AstFuncDecl* node);
    bool emitPublicSwg(Module* moduleToGen, Scope* scope);
};
