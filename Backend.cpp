#include "pch.h"
#include "Backend.h"
#include "Module.h"
#include "Workspace.h"
#include "Version.h"
#include "Ast.h"
#include "AstNode.h"
#include "Scope.h"
#include "ByteCode.h"
#include "TypeManager.h"

void Backend::emitSeparator(Concat& buffer, const char* title)
{
    int len = (int) strlen(title);
    buffer.addString("/*");
    int maxLen = 80;

    int i = 0;
    for (; i < 4; i++)
        buffer.addString("#");
    buffer.addString(" ");
    buffer.addString(title);
    buffer.addString(" ");
    i += len + 2;

    for (; i < maxLen; i++)
        buffer.addString("#");
    buffer.addString("*/\n");
}

void Backend::emitFuncSignatureSwg(TypeInfoFuncAttr* typeFunc, AstFuncDecl* node)
{
    bufferSwg.addString("func ");
    bufferSwg.addString(node->name.c_str());
    bufferSwg.addString("(");

    uint32_t idx = 0;
    for (auto p : typeFunc->parameters)
    {
        bufferSwg.addString(p->namedParam);
        if (p->namedParam != "self")
        {
            bufferSwg.addString(": ");
            bufferSwg.addString(p->typeInfo->name);
        }

        if (idx != typeFunc->parameters.size() - 1)
            bufferSwg.addString(", ");
        idx++;
    }

    bufferSwg.addString(")");

    if (typeFunc->returnType && typeFunc->returnType != g_TypeMgr.typeInfoVoid)
    {
        bufferSwg.addString("->");
        bufferSwg.addString(typeFunc->returnType->name);
    }

    bufferSwg.addString(";");
    bufferSwg.addString("\n");
}

void Backend::emitStructSignatureSwg(TypeInfoStruct* typeStruct, AstStruct* node)
{
    bufferSwg.addString("struct ");
    bufferSwg.addString(node->name.c_str());
    bufferSwg.addString(" {\n");

    for (auto p : typeStruct->childs)
    {
        bufferSwg.addString("\t");
        bufferSwg.addString(p->namedParam);
        bufferSwg.addString(": ");
        bufferSwg.addString(p->typeInfo->name);
        bufferSwg.addString("\n");
    }

    bufferSwg.addString("}\n");
}

void Backend::emitPublicSignaturesSwg(Module* moduleToGen, Scope* scope)
{
    SWAG_ASSERT(moduleToGen);
    if (!scope->hasExports)
        return;

    if (scope->hasExports && !scope->name.empty())
    {
        if (scope->kind == ScopeKind::Namespace)
            bufferSwg.addString(format("namespace %s {\n", scope->name.c_str()));
        else if (scope->kind == ScopeKind::Struct)
            bufferSwg.addString(format("impl %s {\n", scope->name.c_str()));
        else
            bufferSwg.addString("{\n");
    }

    // Structures
    if (!scope->publicStruct.empty())
    {
        bufferSwg.addString("\n");
        for (auto one : scope->publicStruct)
        {
            AstStruct*      node       = CastAst<AstStruct>(one, AstNodeKind::StructDecl);
            TypeInfoStruct* typeStruct = CastTypeInfo<TypeInfoStruct>(node->typeInfo, TypeInfoKind::Struct);
            emitStructSignatureSwg(typeStruct, node);
        }
        bufferSwg.addString("\n");
    }

    // Functions
    if (!scope->publicFunc.empty())
    {
        bufferSwg.addString("\n");
        bufferSwg.addString(format("#[swag.foreign(\"%s\", gen: true)] {\n", module->name.c_str()));
        for (auto func : scope->publicFunc)
        {
            AstFuncDecl*      node     = CastAst<AstFuncDecl>(func, AstNodeKind::FuncDecl);
            TypeInfoFuncAttr* typeFunc = CastTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FuncAttr);
            emitFuncSignatureSwg(typeFunc, node);
        }
        bufferSwg.addString("}\n");
        bufferSwg.addString("\n");
    }

    for (auto oneScope : scope->childScopes)
        emitPublicSignaturesSwg(moduleToGen, oneScope);

    if (scope->hasExports && !scope->name.empty())
        bufferSwg.addString(format("} // %s\n", scope->name.c_str()));
}

bool Backend::preCompile()
{
    auto targetPath    = module->fromTests ? g_Workspace.targetTestPath.string() : g_Workspace.targetPath.string();
    bufferSwg.fileName = targetPath + "\\" + module->name + ".swg";

    bufferSwg.addString(format("/* GENERATED WITH SWAG VERSION %d.%d.%d */\n", SWAG_BUILD_VERSION, SWAG_BUILD_REVISION, SWAG_BUILD_NUM));
    emitPublicSignaturesSwg(module, module->scopeRoot);

    return bufferSwg.flush();
}