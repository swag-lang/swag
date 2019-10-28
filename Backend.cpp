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

bool Backend::emitFuncSwg(TypeInfoFuncAttr* typeFunc, AstFuncDecl* node)
{
    bufferSwg.addString("func ");
    bufferSwg.addString(node->name.c_str());
    bufferSwg.addString("(");

    uint32_t idx = 0;
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

    bufferSwg.addString("\n");
    if (node->content->kind != AstNodeKind::Statement)
        bufferSwg.addString("{\n");
    Ast::output(bufferSwg, node->content);
    if (node->content->kind != AstNodeKind::Statement)
        bufferSwg.addString("}\n");
    bufferSwg.addString("\n");
    return true;
}

bool Backend::emitEnumSignatureSwg(TypeInfoEnum* typeEnum, AstNode* node)
{
    bufferSwg.addString("enum ");
    bufferSwg.addString(node->name.c_str());
    bufferSwg.addString("\n");
    bufferSwg.addString("{\n");

    for (auto p : typeEnum->values)
    {
        bufferSwg.addString("\t");
        bufferSwg.addString(p->namedParam);

        if (typeEnum->rawType->isNative(NativeTypeKind::String))
        {
            bufferSwg.addString(format(" = \"%s\"", p->value.text.c_str()));
        }
        else if (typeEnum->rawType->kind == TypeInfoKind::Native)
        {
            switch (typeEnum->rawType->nativeType)
            {
            case NativeTypeKind::U8:
                bufferSwg.addString(format(" = %u", p->value.reg.u8));
                break;
            case NativeTypeKind::U16:
                bufferSwg.addString(format(" = %u", p->value.reg.u16));
                break;
            case NativeTypeKind::U32:
            case NativeTypeKind::Char:
                bufferSwg.addString(format(" = %u", p->value.reg.u32));
                break;
            case NativeTypeKind::U64:
                bufferSwg.addString(format(" = %llu", p->value.reg.u64));
                break;
            case NativeTypeKind::S8:
                bufferSwg.addString(format(" = %d", p->value.reg.s8));
                break;
            case NativeTypeKind::S16:
                bufferSwg.addString(format(" = %d", p->value.reg.s16));
                break;
            case NativeTypeKind::S32:
                bufferSwg.addString(format(" = %d", p->value.reg.s32));
                break;
            case NativeTypeKind::S64:
                bufferSwg.addString(format(" = %lld", p->value.reg.s64));
                break;
            case NativeTypeKind::F32:
                bufferSwg.addString(format(" = %f", p->value.reg.f32));
                break;
            case NativeTypeKind::F64:
                bufferSwg.addString(format(" = %lf", p->value.reg.f64));
                break;
            case NativeTypeKind::Bool:
                if (p->value.reg.b)
                    bufferSwg.addString(p->value.reg.b ? " = true" : " = false");
                break;
            default:
                return module->internalError("emitEnumSignatureSwg, invalid type");
            }
        }

		bufferSwg.addString("\n");
    }

    bufferSwg.addString("}\n");
    return true;
}

bool Backend::emitStructSignatureSwg(TypeInfoStruct* typeStruct, AstStruct* node)
{
    bufferSwg.addString("struct");

    if (node->genericParameters)
    {
        bufferSwg.addString("(");
        int idx = 0;
        for (auto p : node->genericParameters->childs)
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
    }

    bufferSwg.addString(" ");
    bufferSwg.addString(node->name.c_str());
    bufferSwg.addString("\n");
    bufferSwg.addString("{\n");

    for (auto p : typeStruct->childs)
    {
        bufferSwg.addString("\t");
        bufferSwg.addString(p->namedParam);
        bufferSwg.addString(": ");
        bufferSwg.addString(p->typeInfo->name);

        if (p->typeInfo->isNative(NativeTypeKind::String))
        {
            if (!p->value.text.empty())
                bufferSwg.addString(format(" = \"%s\"", p->value.text.c_str()));
        }
        else if (p->typeInfo->kind == TypeInfoKind::Native)
        {
            switch (p->typeInfo->nativeType)
            {
            case NativeTypeKind::U8:
                if (p->value.reg.u8)
                    bufferSwg.addString(format(" = %u", p->value.reg.u8));
                break;
            case NativeTypeKind::U16:
                if (p->value.reg.u16)
                    bufferSwg.addString(format(" = %u", p->value.reg.u16));
                break;
            case NativeTypeKind::U32:
            case NativeTypeKind::Char:
                if (p->value.reg.u32)
                    bufferSwg.addString(format(" = %u", p->value.reg.u32));
                break;
            case NativeTypeKind::U64:
                if (p->value.reg.u64)
                    bufferSwg.addString(format(" = %llu", p->value.reg.u64));
                break;
            case NativeTypeKind::S8:
                if (p->value.reg.s8)
                    bufferSwg.addString(format(" = %d", p->value.reg.s8));
                break;
            case NativeTypeKind::S16:
                if (p->value.reg.s16)
                    bufferSwg.addString(format(" = %d", p->value.reg.s16));
                break;
            case NativeTypeKind::S32:
                if (p->value.reg.s32)
                    bufferSwg.addString(format(" = %d", p->value.reg.s32));
                break;
            case NativeTypeKind::S64:
                if (p->value.reg.s64)
                    bufferSwg.addString(format(" = %lld", p->value.reg.s64));
                break;
            case NativeTypeKind::F32:
                if (p->value.reg.f32 != 0.0f)
                    bufferSwg.addString(format(" = %f", p->value.reg.f32));
                break;
            case NativeTypeKind::F64:
                if (p->value.reg.f64 != 0.0)
                    bufferSwg.addString(format(" = %lf", p->value.reg.f64));
                break;
            case NativeTypeKind::Bool:
                if (p->value.reg.b)
                    bufferSwg.addString(" = true");
                break;
            default:
                return module->internalError("emitStructSignatureSwg, invalid type");
            }
        }

        bufferSwg.addString("\n");
    }

    bufferSwg.addString("}\n");
    return true;
}

bool Backend::emitPublicSignaturesSwg(Module* moduleToGen, Scope* scope)
{
    SWAG_ASSERT(moduleToGen);
    if (!scope->hasExports)
        return true;

    if (scope->hasExports && !scope->name.empty())
    {
        if (scope->kind == ScopeKind::Namespace)
            bufferSwg.addString(format("namespace %s {\n", scope->name.c_str()));
        else if (scope->kind == ScopeKind::Struct)
            bufferSwg.addString(format("impl %s\n{\n", scope->name.c_str()));
        else
            bufferSwg.addString("{\n");
    }

    // Structures
    if (!scope->publicStruct.empty())
    {
        for (auto one : scope->publicStruct)
        {
            AstStruct*      node       = CastAst<AstStruct>(one, AstNodeKind::StructDecl);
            TypeInfoStruct* typeStruct = CastTypeInfo<TypeInfoStruct>(node->typeInfo, TypeInfoKind::Struct);
            SWAG_CHECK(emitStructSignatureSwg(typeStruct, node));
        }
    }

    // Enums
    if (!scope->publicEnum.empty())
    {
        for (auto one : scope->publicEnum)
        {
            TypeInfoEnum* typeEnum = CastTypeInfo<TypeInfoEnum>(one->typeInfo, TypeInfoKind::Enum);
            SWAG_CHECK(emitEnumSignatureSwg(typeEnum, one));
        }
    }

    // Generic functions
    if (!scope->publicGenericFunc.empty())
    {
        for (auto func : scope->publicGenericFunc)
        {
            AstFuncDecl*      node     = CastAst<AstFuncDecl>(func, AstNodeKind::FuncDecl);
            TypeInfoFuncAttr* typeFunc = CastTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FuncAttr);
            SWAG_CHECK(emitFuncSwg(typeFunc, node));
        }
    }

    // Functions
    if (!scope->publicFunc.empty())
    {
        for (auto func : scope->publicFunc)
        {
            AstFuncDecl*      node     = CastAst<AstFuncDecl>(func, AstNodeKind::FuncDecl);
            TypeInfoFuncAttr* typeFunc = CastTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FuncAttr);
            bufferSwg.addString(format("\t#[swag.foreign(\"%s\", \"%s\")]\n", module->name.c_str(), node->fullnameUnderscore.c_str()));
            bufferSwg.addString("\t");
            SWAG_CHECK(emitFuncSignatureSwg(typeFunc, node));
        }
    }

    for (auto oneScope : scope->childScopes)
        SWAG_CHECK(emitPublicSignaturesSwg(moduleToGen, oneScope));

    if (scope->hasExports && !scope->name.empty())
        bufferSwg.addString("}\n");

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
            if (t1 > module->moreRecentSourceFile)
            {
                return true;
            }
        }
    }

    bufferSwg.addString(format("// GENERATED WITH SWAG VERSION %d.%d.%d\n", SWAG_BUILD_VERSION, SWAG_BUILD_REVISION, SWAG_BUILD_NUM));
    SWAG_CHECK(emitPublicSignaturesSwg(module, module->scopeRoot));

    return bufferSwg.flush();
}