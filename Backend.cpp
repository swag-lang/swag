#include "pch.h"
#include "Backend.h"
#include "Workspace.h"
#include "Version.h"
#include "Ast.h"
#include "Scope.h"
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

bool Backend::emitAttributes(AstNode* node)
{
    if (node->flags & AST_CONST_EXPR)
        bufferSwg.addString("\t#[swag.constexpr]\n");
    if (node->attributeFlags & ATTRIBUTE_INLINE)
        bufferSwg.addString("\t#[swag.inline]\n");
    if (node->attributeFlags & ATTRIBUTE_COMPLETE)
        bufferSwg.addString("\t#[swag.complete]\n");
    return true;
}

bool Backend::emitAttributes(TypeInfoParam* param)
{
    ComputedValue v;
    if (param->attributes.getValue("swag.offset.name", v))
        bufferSwg.addString(format("\t\t#[swag.offset(\"%s\")]\n", v.text.c_str()));

    return true;
}

bool Backend::emitGenericParameters(AstNode* node)
{
    bufferSwg.addString("(");
    int idx = 0;
    for (auto p : node->childs)
    {
        if (idx)
            bufferSwg.addString(", ");
        bufferSwg.addString(p->name);

        AstVarDecl* varDecl = CastAst<AstVarDecl>(p, AstNodeKind::ConstDecl, AstNodeKind::FuncDeclParam);
        if (varDecl->type)
        {
            bufferSwg.addString(": ");
            SWAG_CHECK(Ast::output(bufferSwg, varDecl->type));
        }

        if (varDecl->assignment)
        {
            bufferSwg.addString(" = ");
            SWAG_CHECK(Ast::output(bufferSwg, varDecl->assignment));
        }

        idx++;
    }

    bufferSwg.addString(")");
    return true;
}

bool Backend::emitFuncSignatureSwg(TypeInfoFuncAttr* typeFunc, AstFuncDecl* node)
{
    bufferSwg.addString("func ");
    bufferSwg.addString(node->name.c_str());
    bufferSwg.addString("(");

    if (node->parameters)
    {
        uint32_t idx = 0;
        for (auto p : node->parameters->childs)
        {
            bufferSwg.addString(p->name);
            if (p->name != "self")
            {
                bufferSwg.addString(": ");
                bufferSwg.addString(p->typeInfo->name);
            }

            AstVarDecl* varDecl = CastAst<AstVarDecl>(p, AstNodeKind::VarDecl, AstNodeKind::FuncDeclParam);
            if (varDecl->assignment)
            {
                bufferSwg.addString(" = ");
                Ast::output(bufferSwg, varDecl->assignment);
            }

            if (idx != node->parameters->childs.size() - 1)
                bufferSwg.addString(", ");
            idx++;
        }
    }

    bufferSwg.addString(")");

    if (typeFunc->returnType && typeFunc->returnType != g_TypeMgr.typeInfoVoid)
    {
        bufferSwg.addString("->");
        bufferSwg.addString(typeFunc->returnType->name);
    }

    bufferSwg.addString(";");
    bufferSwg.addString("\n");
    return true;
}

bool Backend::emitPublicFuncSwg(TypeInfoFuncAttr* typeFunc, AstFuncDecl* node)
{
    SWAG_CHECK(emitAttributes(node));
    bufferSwg.addString("\tfunc");
    if (node->genericParameters)
        SWAG_CHECK(emitGenericParameters(node->genericParameters));

    bufferSwg.addString(" ");
    bufferSwg.addString(node->name.c_str());
    bufferSwg.addString("(");

    uint32_t idx = 0;
    if (node->parameters)
    {
        for (auto p : node->parameters->childs)
        {
            if (p->flags & AST_DECL_USING)
                bufferSwg.addString("using ");

            bufferSwg.addString(p->name);
            if (p->name != "self")
            {
                bufferSwg.addString(": ");
                bufferSwg.addString(p->typeInfo->name);
            }

            auto param = CastAst<AstVarDecl>(p, AstNodeKind::FuncDeclParam);
            if (param->assignment)
            {
                bufferSwg.addString(" = ");
                SWAG_CHECK(Ast::output(bufferSwg, param->assignment));
            }

            if (idx != typeFunc->parameters.size() - 1)
                bufferSwg.addString(", ");
            idx++;
        }
    }

    bufferSwg.addString(")");

    if (typeFunc->returnType && typeFunc->returnType != g_TypeMgr.typeInfoVoid)
    {
        bufferSwg.addString("->");
        bufferSwg.addString(typeFunc->returnType->name);
    }

    bufferSwg.addEolIndent(1);
    if (node->content->kind != AstNodeKind::Statement)
        bufferSwg.addString("{\n\t\t");
    Ast::output(bufferSwg, node->content, 1);
    if (node->content->kind != AstNodeKind::Statement)
        bufferSwg.addString("\n\t}\n");
    bufferSwg.addString("\n");
    return true;
}

bool Backend::emitPublicEnumSwg(TypeInfoEnum* typeEnum, AstNode* node)
{
    bufferSwg.addString("\tenum ");
    bufferSwg.addString(node->name.c_str());
    bufferSwg.addString(" : ");
    bufferSwg.addString(typeEnum->rawType->name);

    bufferSwg.addString("\n\t{\n");

    for (auto p : typeEnum->values)
    {
        bufferSwg.addString("\t\t");
        bufferSwg.addString(p->namedParam);
        bufferSwg.addString(" = ");
        SWAG_CHECK(Ast::outputLiteral(bufferSwg, node, typeEnum->rawType, p->value.text, p->value.reg));
        bufferSwg.addString("\n");
    }

    bufferSwg.addString("\t}\n\n");
    return true;
}

bool Backend::emitPublicConstSwg(AstVarDecl* node)
{
    bufferSwg.addString("\tconst ");
    bufferSwg.addString(node->name.c_str());
    bufferSwg.addString(": ");
    bufferSwg.addString(node->typeInfo->name);
    bufferSwg.addString(" = ");
    SWAG_CHECK(Ast::outputLiteral(bufferSwg, node, node->assignment->typeInfo, node->assignment->computedValue.text, node->assignment->computedValue.reg));
    bufferSwg.addString("\n");
    return true;
}

bool Backend::emitPublicStructSwg(TypeInfoStruct* typeStruct, AstStruct* node)
{
    bufferSwg.addString("\tstruct");
    if (node->genericParameters)
        SWAG_CHECK(emitGenericParameters(node->genericParameters));

    bufferSwg.addString(" ");
    bufferSwg.addString(node->name.c_str());
    bufferSwg.addString("\n\t{\n");

    for (auto p : typeStruct->childs)
    {
        SWAG_CHECK(emitAttributes(p));
        bufferSwg.addString("\t\t");
        bufferSwg.addString(p->namedParam);
        bufferSwg.addString(": ");
        bufferSwg.addString(p->typeInfo->name);

        if (p->typeInfo->kind == TypeInfoKind::Native)
        {
            bool notZero = false;
            switch (p->typeInfo->nativeType)
            {
            case NativeTypeKind::U8:
            case NativeTypeKind::S8:
                notZero = p->value.reg.u8;
                break;
            case NativeTypeKind::U16:
            case NativeTypeKind::S16:
                notZero = p->value.reg.u16;
                break;
            case NativeTypeKind::U32:
            case NativeTypeKind::Char:
            case NativeTypeKind::S32:
            case NativeTypeKind::F32:
                notZero = p->value.reg.u32;
                break;
            case NativeTypeKind::U64:
            case NativeTypeKind::S64:
            case NativeTypeKind::F64:
                notZero = p->value.reg.u64;
                break;
            case NativeTypeKind::Bool:
                notZero = p->value.reg.b;
                break;
            case NativeTypeKind::String:
                notZero = !p->value.text.empty();
                break;
            }

            if (notZero)
            {
				bufferSwg.addString(" = ");
                SWAG_CHECK(Ast::outputLiteral(bufferSwg, node, p->typeInfo, p->value.text, p->value.reg));
            }
        }

        bufferSwg.addString("\n");
    }

    bufferSwg.addString("\t}\n\n");
    return true;
}

bool Backend::emitPublicSwg(Module* moduleToGen, Scope* scope)
{
    SWAG_ASSERT(moduleToGen);
    if (!scope->hasExports)
        return true;

    if (scope->hasExports && !scope->name.empty())
    {
        if (scope->kind == ScopeKind::Namespace)
            bufferSwg.addString(format("namespace %s {\n", scope->name.c_str()));
        else if (scope->kind == ScopeKind::Struct)
            bufferSwg.addString(format("impl %s {\n", scope->name.c_str()));
        else
            bufferSwg.addString("{\n");
    }

    // Enums
    if (!scope->publicConst.empty())
    {
        for (auto one : scope->publicConst)
        {
            AstVarDecl* node = CastAst<AstVarDecl>(one, AstNodeKind::ConstDecl);
            SWAG_CHECK(emitPublicConstSwg(node));
        }
    }

    // Structures
    if (!scope->publicStruct.empty())
    {
        for (auto one : scope->publicStruct)
        {
            AstStruct*      node       = CastAst<AstStruct>(one, AstNodeKind::StructDecl);
            TypeInfoStruct* typeStruct = CastTypeInfo<TypeInfoStruct>(node->typeInfo, TypeInfoKind::Struct);
            SWAG_CHECK(emitPublicStructSwg(typeStruct, node));
        }
    }

    // Enums
    if (!scope->publicEnum.empty())
    {
        for (auto one : scope->publicEnum)
        {
            TypeInfoEnum* typeEnum = CastTypeInfo<TypeInfoEnum>(one->typeInfo, TypeInfoKind::Enum);
            SWAG_CHECK(emitPublicEnumSwg(typeEnum, one));
        }
    }

    // Generic functions
    if (!scope->publicGenericFunc.empty())
    {
        for (auto func : scope->publicGenericFunc)
        {
            AstFuncDecl*      node     = CastAst<AstFuncDecl>(func, AstNodeKind::FuncDecl);
            TypeInfoFuncAttr* typeFunc = CastTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FuncAttr);
            SWAG_CHECK(emitPublicFuncSwg(typeFunc, node));
        }
    }

    // Functions
    if (!scope->publicFunc.empty())
    {
        for (auto func : scope->publicFunc)
        {
            AstFuncDecl*      node     = CastAst<AstFuncDecl>(func, AstNodeKind::FuncDecl);
            TypeInfoFuncAttr* typeFunc = CastTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FuncAttr);
            bufferSwg.addString(format("\t#[swag.foreign(\"%s\", \"%s\")]\n", module->name.c_str(), node->fullnameForeign.c_str()));
            SWAG_CHECK(emitAttributes(node));
            bufferSwg.addString("\t");
            SWAG_CHECK(emitFuncSignatureSwg(typeFunc, node));
        }
    }

    for (auto oneScope : scope->childScopes)
        SWAG_CHECK(emitPublicSwg(moduleToGen, oneScope));

    if (scope->hasExports && !scope->name.empty())
        bufferSwg.addString("}\n\n");

    return true;
}

bool Backend::preCompile()
{
    auto targetPath    = module->fromTests ? g_Workspace.targetTestPath.string() : g_Workspace.targetPath.string();
    bufferSwg.fileName = targetPath + "\\" + module->name + ".swg";

    // Do we need to generate the file ?
    if (!g_CommandLine.rebuild)
    {
        if (fs::exists(bufferSwg.fileName))
        {
            fs::file_time_type mtime = fs::last_write_time(bufferSwg.fileName);
            time_t             t1    = fs::file_time_type::clock::to_time_t(mtime);
            if (t1 > module->moreRecentSourceFile && t1 > g_Workspace.runtimeModule->moreRecentSourceFile)
            {
                return true;
            }
        }
    }

    bufferSwg.addString(format("// GENERATED WITH SWAG VERSION %d.%d.%d\n", SWAG_BUILD_VERSION, SWAG_BUILD_REVISION, SWAG_BUILD_NUM));

    SWAG_CHECK(emitPublicSwg(module, module->scopeRoot));

    return bufferSwg.flush();
}