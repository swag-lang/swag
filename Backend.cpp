#include "pch.h"
#include "Backend.h"
#include "Module.h"
#include "Workspace.h"
#include "Version.h"
#include "Ast.h"
#include "AstNode.h"
#include "Scope.h"
#include "ByteCode.h"

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
    if (!(node->attributeFlags & ATTRIBUTE_PUBLIC))
        return;

    if (!node->ownerScope->fullname.empty())
        bufferSwg.addString(format("namespace %s {\n", node->ownerScope->fullname.c_str()));

    bufferSwg.addString(format("#[swag.foreign(\"%s\", gen: true)]\n", module->name.c_str()));
    bufferSwg.addString("func ");
    bufferSwg.addString(node->name.c_str());
    bufferSwg.addString("(");
    bufferSwg.addString(")");

    bufferSwg.addString(";");
    if (!node->ownerScope->fullname.empty())
        bufferSwg.addString(" }");
    bufferSwg.addString("\n");
}

bool Backend::emitFuncSignaturesSwg(Module* moduleToGen)
{
    SWAG_ASSERT(moduleToGen);

    bufferSwg.addString(format("#[swag.foreign(\"%s\", gen: true)]\n", module->name.c_str()));
    bufferSwg.addString("{\n");

    for (auto one : moduleToGen->byteCodeFunc)
    {
        TypeInfoFuncAttr* typeFunc = one->typeInfoFunc;
        AstFuncDecl*      node     = nullptr;

        if (one->node)
        {
            node = CastAst<AstFuncDecl>(one->node, AstNodeKind::FuncDecl);

            // Do we need to generate that function ?
            if (node->attributeFlags & ATTRIBUTE_COMPILER)
                continue;
            if ((node->attributeFlags & ATTRIBUTE_TEST_FUNC) && !g_CommandLine.test)
                continue;
            if (node->attributeFlags & ATTRIBUTE_FOREIGN)
                continue;

            typeFunc = CastTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FuncAttr);
        }

        if (node && (node->attributeFlags & ATTRIBUTE_PUBLIC))
            emitFuncSignatureSwg(typeFunc, node);
    }

    bufferSwg.addString("}\n");
    return true;
}

bool Backend::emitFuncSignaturesSwg()
{
    emitSeparator(bufferSwg, "PROTOTYPES");
    if (!emitFuncSignaturesSwg(g_Workspace.runtimeModule))
        return false;
    if (!emitFuncSignaturesSwg(module))
        return false;
    return true;
}

bool Backend::preCompile()
{
    auto targetPath    = module->fromTests ? g_Workspace.targetTestPath.string() : g_Workspace.targetPath.string();
    bufferSwg.fileName = targetPath + "\\" + module->name + ".swg";

	bufferSwg.addString(format("/* GENERATED WITH SWAG VERSION %d.%d.%d */\n", SWAG_BUILD_VERSION, SWAG_BUILD_REVISION, SWAG_BUILD_NUM));

	SWAG_CHECK(emitFuncSignaturesSwg());

    return bufferSwg.flush();
}