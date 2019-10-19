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
    if (!(node->attributeFlags & ATTRIBUTE_PUBLIC))
        return;

    bufferSwg.addString("func ");
    bufferSwg.addString(node->name.c_str());
    bufferSwg.addString("(");

    uint32_t idx = 0;
    for (auto p : typeFunc->parameters)
    {
        bufferSwg.addString(p->namedParam);
        bufferSwg.addString(": ");
        bufferSwg.addString(p->typeInfo->name);

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

void Backend::emitFuncSignaturesSwg(Module* moduleToGen, Scope* scope)
{
    SWAG_ASSERT(moduleToGen);
    if (!scope->hasExports)
        return;

    if (scope->hasExports && !scope->name.empty())
        bufferSwg.addString(format("namespace %s {\n", scope->name.c_str()));

	if (!scope->publicFunc.empty())
	{
		bufferSwg.addString(format("#[swag.foreign(\"%s\", gen: true)] {\n", module->name.c_str()));
		for (auto func : scope->publicFunc)
		{
			AstFuncDecl* node = CastAst<AstFuncDecl>(func, AstNodeKind::FuncDecl);
			TypeInfoFuncAttr* typeFunc = CastTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FuncAttr);
			emitFuncSignatureSwg(typeFunc, node);
		}
		bufferSwg.addString("}\n");
	}

    for (auto oneScope : scope->childScopes)
        emitFuncSignaturesSwg(moduleToGen, oneScope);

    if (scope->hasExports && !scope->name.empty())
        bufferSwg.addString(format("} // namespace %s\n", scope->name.c_str()));
}

bool Backend::preCompile()
{
    auto targetPath    = module->fromTests ? g_Workspace.targetTestPath.string() : g_Workspace.targetPath.string();
    bufferSwg.fileName = targetPath + "\\" + module->name + ".swg";

    bufferSwg.addString(format("/* GENERATED WITH SWAG VERSION %d.%d.%d */\n", SWAG_BUILD_VERSION, SWAG_BUILD_REVISION, SWAG_BUILD_NUM));
    emitSeparator(bufferSwg, "PROTOTYPES");
    emitFuncSignaturesSwg(module, module->scopeRoot);

    return bufferSwg.flush();
}