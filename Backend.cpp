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
	CONCAT_FIXED_STR(buffer, "/*");
    int maxLen = 80;

    int i = 0;
    for (; i < 4; i++)
		CONCAT_FIXED_STR(buffer, "#");
	CONCAT_FIXED_STR(buffer, " ");
    buffer.addString(title);
	CONCAT_FIXED_STR(buffer, " ");
    i += len + 2;

    for (; i < maxLen; i++)
		CONCAT_FIXED_STR(buffer, "#");
	CONCAT_FIXED_STR(buffer, "*/\n");
}

bool Backend::emitAttributes(AstNode* node)
{
    if (node->flags & AST_CONST_EXPR)
		CONCAT_FIXED_STR(bufferSwg, "\t#[constexpr]\n");
    if (node->attributeFlags & ATTRIBUTE_INLINE)
		CONCAT_FIXED_STR(bufferSwg, "\t#[inline]\n");
    if (node->attributeFlags & ATTRIBUTE_COMPLETE)
		CONCAT_FIXED_STR(bufferSwg, "\t#[complete]\n");
    return true;
}

bool Backend::emitAttributes(TypeInfoParam* param)
{
    ComputedValue v;
    if (param->attributes.getValue("swag.offset.name", v))
        bufferSwg.addStringFormat("\t\t#[offset(\"%s\")]\n", v.text.c_str());

    return true;
}

bool Backend::emitGenericParameters(AstNode* node)
{
	CONCAT_FIXED_STR(bufferSwg, "(");
    int idx = 0;
    for (auto p : node->childs)
    {
        if (idx)
			CONCAT_FIXED_STR(bufferSwg, ", ");
        bufferSwg.addString(p->name);

        AstVarDecl* varDecl = CastAst<AstVarDecl>(p, AstNodeKind::ConstDecl, AstNodeKind::FuncDeclParam);
        if (varDecl->type)
        {
			CONCAT_FIXED_STR(bufferSwg, ": ");
            SWAG_CHECK(Ast::output(bufferSwg, varDecl->type));
        }

        if (varDecl->assignment)
        {
			CONCAT_FIXED_STR(bufferSwg, " = ");
            SWAG_CHECK(Ast::output(bufferSwg, varDecl->assignment));
        }

        idx++;
    }

	CONCAT_FIXED_STR(bufferSwg, ")");
    return true;
}

bool Backend::emitFuncSignatureSwg(TypeInfoFuncAttr* typeFunc, AstFuncDecl* node)
{
	CONCAT_FIXED_STR(bufferSwg, "func ");
    bufferSwg.addString(node->name.c_str());
	CONCAT_FIXED_STR(bufferSwg, "(");

    if (node->parameters)
    {
        uint32_t idx = 0;
        for (auto p : node->parameters->childs)
        {
            bufferSwg.addString(p->name);
            if (p->name != "self")
            {
				CONCAT_FIXED_STR(bufferSwg, ": ");
                bufferSwg.addString(p->typeInfo->name);
            }

            AstVarDecl* varDecl = CastAst<AstVarDecl>(p, AstNodeKind::VarDecl, AstNodeKind::FuncDeclParam);
            if (varDecl->assignment)
            {
				CONCAT_FIXED_STR(bufferSwg, " = ");
                Ast::output(bufferSwg, varDecl->assignment);
            }

            if (idx != node->parameters->childs.size() - 1)
				CONCAT_FIXED_STR(bufferSwg, ", ");
            idx++;
        }
    }

	CONCAT_FIXED_STR(bufferSwg, ")");

    if (typeFunc->returnType && typeFunc->returnType != g_TypeMgr.typeInfoVoid)
    {
		CONCAT_FIXED_STR(bufferSwg, "->");
        if (typeFunc->returnType->kind == TypeInfoKind::Struct)
        {
            // Just to remove the "const"
            auto typeStruct = CastTypeInfo<TypeInfoStruct>(typeFunc->returnType, TypeInfoKind::Struct);
            bufferSwg.addString(typeStruct->structNode->name);
        }
        else
            bufferSwg.addString(typeFunc->returnType->name);
    }

	CONCAT_FIXED_STR(bufferSwg, ";");
	CONCAT_FIXED_STR(bufferSwg, "\n");
    return true;
}

bool Backend::emitPublicFuncSwg(TypeInfoFuncAttr* typeFunc, AstFuncDecl* node)
{
    SWAG_CHECK(emitAttributes(node));
	CONCAT_FIXED_STR(bufferSwg, "\tfunc");
    if (node->genericParameters)
        SWAG_CHECK(emitGenericParameters(node->genericParameters));

	CONCAT_FIXED_STR(bufferSwg, " ");
    bufferSwg.addString(node->name.c_str());
	CONCAT_FIXED_STR(bufferSwg, "(");

    uint32_t idx = 0;
    if (node->parameters)
    {
        for (auto p : node->parameters->childs)
        {
            if (p->flags & AST_DECL_USING)
				CONCAT_FIXED_STR(bufferSwg, "using ");

            bufferSwg.addString(p->name);
            if (p->name != "self")
            {
				CONCAT_FIXED_STR(bufferSwg, ": ");
                bufferSwg.addString(p->typeInfo->name);
            }

            auto param = CastAst<AstVarDecl>(p, AstNodeKind::FuncDeclParam);
            if (param->assignment)
            {
				CONCAT_FIXED_STR(bufferSwg, " = ");
                SWAG_CHECK(Ast::output(bufferSwg, param->assignment));
            }

            if (idx != typeFunc->parameters.size() - 1)
				CONCAT_FIXED_STR(bufferSwg, ", ");
            idx++;
        }
    }

	CONCAT_FIXED_STR(bufferSwg, ")");

    if (typeFunc->returnType && typeFunc->returnType != g_TypeMgr.typeInfoVoid)
    {
		CONCAT_FIXED_STR(bufferSwg, "->");
        bufferSwg.addString(typeFunc->returnType->name);
    }

    bufferSwg.addEolIndent(1);
    if (node->content->kind != AstNodeKind::Statement)
		CONCAT_FIXED_STR(bufferSwg, "{\n\t\t");
    Ast::output(bufferSwg, node->content, 1);
    if (node->content->kind != AstNodeKind::Statement)
		CONCAT_FIXED_STR(bufferSwg, "\n\t}\n");
	CONCAT_FIXED_STR(bufferSwg, "\n");
    return true;
}

bool Backend::emitPublicEnumSwg(TypeInfoEnum* typeEnum, AstNode* node)
{
	CONCAT_FIXED_STR(bufferSwg, "\tenum ");
    bufferSwg.addString(node->name.c_str());
	CONCAT_FIXED_STR(bufferSwg, " : ");
    bufferSwg.addString(typeEnum->rawType->name);

	CONCAT_FIXED_STR(bufferSwg, "\n\t{\n");

    for (auto p : typeEnum->values)
    {
		CONCAT_FIXED_STR(bufferSwg, "\t\t");
        bufferSwg.addString(p->namedParam);
		CONCAT_FIXED_STR(bufferSwg, " = ");
        SWAG_CHECK(Ast::outputLiteral(bufferSwg, node, typeEnum->rawType, p->value.text, p->value.reg));
		CONCAT_FIXED_STR(bufferSwg, "\n");
    }

	CONCAT_FIXED_STR(bufferSwg, "\t}\n\n");
    return true;
}

bool Backend::emitPublicConstSwg(AstVarDecl* node)
{
	CONCAT_FIXED_STR(bufferSwg, "\tconst ");
    bufferSwg.addString(node->name.c_str());

    if (node->type)
    {
		CONCAT_FIXED_STR(bufferSwg, ": ");
        SWAG_CHECK(Ast::output(bufferSwg, node->type));
    }

    if (node->assignment)
    {
		CONCAT_FIXED_STR(bufferSwg, " = ");
        SWAG_CHECK(Ast::outputLiteral(bufferSwg, node->assignment, node->assignment->typeInfo, node->assignment->computedValue.text, node->assignment->computedValue.reg));
    }

    bufferSwg.addEol();
    return true;
}

bool Backend::emitPublicTypeAliasSwg(AstNode* node)
{
	CONCAT_FIXED_STR(bufferSwg, "\ttypealias ");
    bufferSwg.addString(node->name.c_str());
	CONCAT_FIXED_STR(bufferSwg, ": ");
    SWAG_CHECK(Ast::output(bufferSwg, node->childs.front()));

	CONCAT_FIXED_STR(bufferSwg, "\n");
    return true;
}

bool Backend::emitPublicStructSwg(TypeInfoStruct* typeStruct, AstStruct* node)
{
	CONCAT_FIXED_STR(bufferSwg, "\tstruct");
    if (node->genericParameters)
        SWAG_CHECK(emitGenericParameters(node->genericParameters));

	CONCAT_FIXED_STR(bufferSwg, " ");
    bufferSwg.addString(node->name.c_str());
	CONCAT_FIXED_STR(bufferSwg, "\n\t{\n");

    for (auto p : typeStruct->childs)
    {
        SWAG_CHECK(emitAttributes(p));
		CONCAT_FIXED_STR(bufferSwg, "\t\t");
        bufferSwg.addString(p->namedParam);
		CONCAT_FIXED_STR(bufferSwg, ": ");
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
				CONCAT_FIXED_STR(bufferSwg, " = ");
                SWAG_CHECK(Ast::outputLiteral(bufferSwg, node, p->typeInfo, p->value.text, p->value.reg));
            }
        }

		CONCAT_FIXED_STR(bufferSwg, "\n");
    }

	CONCAT_FIXED_STR(bufferSwg, "\t}\n\n");
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
            bufferSwg.addStringFormat("namespace %s {\n", scope->name.c_str());
        else if (scope->kind == ScopeKind::Struct)
            bufferSwg.addStringFormat("impl %s {\n", scope->name.c_str());
        else
			CONCAT_FIXED_STR(bufferSwg, "{\n");
    }

    // Consts
    if (!scope->publicConst.empty())
    {
        for (auto one : scope->publicConst)
        {
            AstVarDecl* node = CastAst<AstVarDecl>(one, AstNodeKind::ConstDecl);
            SWAG_CHECK(emitPublicConstSwg(node));
        }
    }

    // Typealias
    if (!scope->publicTypeAlias.empty())
    {
        for (auto one : scope->publicTypeAlias)
        {
            SWAG_CHECK(emitPublicTypeAliasSwg(one));
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
            bufferSwg.addStringFormat("\t#[foreign(\"%s\", \"%s\")]\n", module->name.c_str(), node->fullnameForeign.c_str());
            SWAG_CHECK(emitAttributes(node));
            bufferSwg.addChar('\t');
            SWAG_CHECK(emitFuncSignatureSwg(typeFunc, node));
        }
    }

    for (auto oneScope : scope->childScopes)
        SWAG_CHECK(emitPublicSwg(moduleToGen, oneScope));

    if (scope->hasExports && !scope->name.empty())
		CONCAT_FIXED_STR(bufferSwg, "}\n\n");

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

    bufferSwg.addStringFormat("// GENERATED WITH SWAG VERSION %d.%d.%d\n", SWAG_BUILD_VERSION, SWAG_BUILD_REVISION, SWAG_BUILD_NUM);

    for (auto depName : module->moduleDependenciesNames)
        bufferSwg.addStringFormat("#import \"%s\"\n", depName.c_str());
	CONCAT_FIXED_STR(bufferSwg, "using swag\n");

    // Emit everything that's public
    SWAG_CHECK(emitPublicSwg(module, module->scopeRoot));

    return bufferSwg.flush();
}