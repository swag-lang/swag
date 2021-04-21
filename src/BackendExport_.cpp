#include "pch.h"
#include "Backend.h"
#include "Workspace.h"
#include "Version.h"
#include "TypeManager.h"
#include "Os.h"
#include "Module.h"
#include "Profile.h"
#include "ModuleSaveExportJob.h"
#include "AstNode.h"
#include "ByteCode.h"

bool Backend::emitAttributesUsage(TypeInfoFuncAttr* typeFunc, int indent)
{
    bool first = true;
    bufferSwg.addIndent(indent);
    bufferSwg.addString("#[attributeUsage(");

#define ADD_ATTRUSAGE(__f, __n)                         \
    if (typeFunc->attributeUsage & (int) __f)           \
    {                                                   \
        if (!first)                                     \
            CONCAT_FIXED_STR(bufferSwg, "|");           \
        first = false;                                  \
        CONCAT_FIXED_STR(bufferSwg, "AttributeUsage."); \
        CONCAT_FIXED_STR(bufferSwg, __n);               \
    }

    if ((typeFunc->attributeUsage & AttributeUsage::All) == AttributeUsage::All)
        CONCAT_FIXED_STR(bufferSwg, "AttributeUsage.All");
    else
    {
        ADD_ATTRUSAGE(AttributeUsage::Enum, "Enum");
        ADD_ATTRUSAGE(AttributeUsage::EnumValue, "EnumValue");
        ADD_ATTRUSAGE(AttributeUsage::StructVariable, "Field");
        ADD_ATTRUSAGE(AttributeUsage::GlobalVariable, "GlobalVariable");
        ADD_ATTRUSAGE(AttributeUsage::Struct, "Struct");
        ADD_ATTRUSAGE(AttributeUsage::Function, "Function");
    }

    bufferSwg.addString(")]");
    bufferSwg.addEol();
    return true;
}

bool Backend::emitAttributes(TypeInfo* typeInfo, int indent)
{
    SymbolAttributes* attr = nullptr;
    switch (typeInfo->kind)
    {
    case TypeInfoKind::Struct:
    case TypeInfoKind::TypeSet:
    case TypeInfoKind::Interface:
    {
        auto type = CastTypeInfo<TypeInfoStruct>(typeInfo, typeInfo->kind);
        attr      = &type->attributes;
        break;
    }
    case TypeInfoKind::FuncAttr:
    {
        auto type = CastTypeInfo<TypeInfoFuncAttr>(typeInfo, typeInfo->kind);
        attr      = &type->attributes;
        break;
    }
    case TypeInfoKind::Enum:
    {
        auto type = CastTypeInfo<TypeInfoEnum>(typeInfo, typeInfo->kind);
        attr      = &type->attributes;
        break;
    }
    case TypeInfoKind::Param:
    {
        auto type = CastTypeInfo<TypeInfoParam>(typeInfo, typeInfo->kind);
        attr      = &type->attributes;
        break;
    }
    }

    outputContext.indent = indent;
    SWAG_CHECK(Ast::outputAttributes(outputContext, bufferSwg, *attr));
    return true;
}

bool Backend::emitTypeTuple(TypeInfo* typeInfo, int indent)
{
    typeInfo = TypeManager::concreteReference(typeInfo);
    SWAG_ASSERT(typeInfo->flags & TYPEINFO_STRUCT_IS_TUPLE);
    auto typeStruct = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
    auto nodeStruct = CastAst<AstStruct>(typeStruct->declNode, AstNodeKind::StructDecl);
    if (nodeStruct->structFlags & STRUCTFLAG_ANONYMOUS)
    {
        SWAG_CHECK(emitPublicStructSwg(typeStruct, (AstStruct*) typeStruct->declNode, indent));
        return true;
    }

    bufferSwg.addString("{");
    int idx = 0;
    for (auto field : typeStruct->fields)
    {
        if (idx)
            CONCAT_FIXED_STR(bufferSwg, ", ");

        if (field->declNode && field->declNode->kind == AstNodeKind::VarDecl)
        {
            auto varDecl = CastAst<AstVarDecl>(field->declNode, AstNodeKind::VarDecl);
            SWAG_CHECK(emitVarSwg(nullptr, varDecl, 0));
        }
        else
        {
            if (!field->namedParam.empty() && field->namedParam.find("item") != 0)
            {
                bufferSwg.addString(field->namedParam);
                bufferSwg.addString(":");
            }

            emitType(field->typeInfo, indent);
        }

        idx++;
    }

    bufferSwg.addString("}");
    return true;
}

void Backend::emitType(TypeInfo* typeInfo, int indent)
{
    if (typeInfo->kind == TypeInfoKind::Lambda)
    {
        auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(typeInfo, TypeInfoKind::Lambda);
        CONCAT_FIXED_STR(bufferSwg, "func(");
        for (auto p : typeFunc->parameters)
        {
            if (p != typeFunc->parameters[0])
                CONCAT_FIXED_STR(bufferSwg, ", ");
            emitType(p->typeInfo, indent);
        }

        CONCAT_FIXED_STR(bufferSwg, ")->");
        emitType(typeFunc->returnType, indent);

        if (typeInfo->flags & TYPEINFO_CAN_THROW)
            CONCAT_FIXED_STR(bufferSwg, " throw");
    }
    else
    {
        if (typeInfo->flags & TYPEINFO_STRUCT_IS_TUPLE)
        {
            emitTypeTuple(typeInfo, indent);
            return;
        }

        // Be sure to keep the original reference. That way, only user references are exported as references, otherwise
        // we take the 'converted' original struct type
        if (typeInfo->kind == TypeInfoKind::Reference)
        {
            auto typeRef = CastTypeInfo<TypeInfoReference>(typeInfo, TypeInfoKind::Reference);
            if (typeRef->originalType)
                typeInfo = typeRef->originalType;
        }

        if (typeInfo->flags & TYPEINFO_SELF)
        {
            if (typeInfo->flags & TYPEINFO_CONST)
                CONCAT_FIXED_STR(bufferSwg, "const self");
            else
                CONCAT_FIXED_STR(bufferSwg, "self");
        }
        else
        {
            typeInfo->computeScopedNameExport();
            SWAG_ASSERT(!typeInfo->scopedNameExport.empty());
            bufferSwg.addString(typeInfo->scopedNameExport);
        }
    }
}

bool Backend::emitGenericParameters(AstNode* node, int indent)
{
    CONCAT_FIXED_STR(bufferSwg, "(");
    int idx = 0;
    for (auto p : node->childs)
    {
        if (idx)
            CONCAT_FIXED_STR(bufferSwg, ", ");
        bufferSwg.addString(p->token.text);

        AstVarDecl* varDecl = CastAst<AstVarDecl>(p, AstNodeKind::ConstDecl, AstNodeKind::FuncDeclParam);
        if (varDecl->type)
        {
            CONCAT_FIXED_STR(bufferSwg, ": ");
            emitType(varDecl->type->typeInfo, indent);
        }

        if (varDecl->assignment)
        {
            CONCAT_FIXED_STR(bufferSwg, " = ");
            SWAG_CHECK(Ast::output(outputContext, bufferSwg, varDecl->assignment));
        }

        idx++;
    }

    CONCAT_FIXED_STR(bufferSwg, ")");
    return true;
}

bool Backend::emitFuncSignatureSwg(TypeInfoFuncAttr* typeFunc, AstFuncDecl* node, int indent)
{
    return emitFuncSignatureSwg(typeFunc, node, node->parameters, node->selectIf, indent);
}

bool Backend::emitFuncSignatureSwg(TypeInfoFuncAttr* typeFunc, AstNode* node, AstNode* parameters, AstNode* selectIf, int indent)
{
    if (node->kind == AstNodeKind::AttrDecl)
        CONCAT_FIXED_STR(bufferSwg, "attr ");
    else
        CONCAT_FIXED_STR(bufferSwg, "func ");
    bufferSwg.addString(node->token.text.c_str());
    CONCAT_FIXED_STR(bufferSwg, "(");

    if (parameters)
    {
        uint32_t idx = 0;
        for (auto p : typeFunc->parameters)
        {
            AstVarDecl* varDecl = CastAst<AstVarDecl>(parameters->childs[idx], AstNodeKind::VarDecl, AstNodeKind::FuncDeclParam);

            // Name
            if (varDecl->token.text == "self" && p->typeInfo->isConst())
                bufferSwg.addString("const ");

            bufferSwg.addString(varDecl->token.text);

            // Type
            if (varDecl->token.text != "self")
            {
                CONCAT_FIXED_STR(bufferSwg, ": ");
                emitType(p->typeInfo, indent);
            }

            // Assignment
            if (varDecl->assignment)
            {
                CONCAT_FIXED_STR(bufferSwg, " = ");
                SWAG_CHECK(Ast::output(outputContext, bufferSwg, varDecl->assignment));
            }

            if (idx != parameters->childs.size() - 1)
                CONCAT_FIXED_STR(bufferSwg, ", ");
            idx++;
        }
    }

    CONCAT_FIXED_STR(bufferSwg, ")");

    if (typeFunc->returnType && typeFunc->returnType != g_TypeMgr.typeInfoVoid)
    {
        CONCAT_FIXED_STR(bufferSwg, "->");
        emitType(typeFunc->returnType, indent);
    }

    if (typeFunc->flags & TYPEINFO_CAN_THROW)
        CONCAT_FIXED_STR(bufferSwg, " throw");

    if (selectIf)
    {
        bufferSwg.addEolIndent(indent + 1);
        outputContext.indent++;
        SWAG_CHECK(Ast::output(outputContext, bufferSwg, selectIf));
        outputContext.indent--;
    }

    CONCAT_FIXED_STR(bufferSwg, ";");
    bufferSwg.addEol();
    return true;
}

bool Backend::emitPublicFuncSwg(TypeInfoFuncAttr* typeFunc, AstFuncDecl* node, int indent)
{
    SWAG_CHECK(emitAttributes(typeFunc, indent));
    bufferSwg.addIndent(indent);
    CONCAT_FIXED_STR(bufferSwg, "func");

    // Emit generic parameter, except if the function is an instance
    if (node->genericParameters)
    {
        if (!(node->flags & AST_FROM_GENERIC) || (node->flags & AST_IS_GENERIC))
            SWAG_CHECK(emitGenericParameters(node->genericParameters, indent));
    }

    CONCAT_FIXED_STR(bufferSwg, " ");
    bufferSwg.addString(node->token.text.c_str());
    CONCAT_FIXED_STR(bufferSwg, "(");

    uint32_t idx = 0;
    if (node->parameters)
    {
        for (auto p : node->parameters->childs)
        {
            if (p->flags & AST_DECL_USING)
                CONCAT_FIXED_STR(bufferSwg, "using ");

            if (p->token.text == "self" && p->typeInfo->isConst())
                bufferSwg.addString("const ");

            bufferSwg.addString(p->token.text);

            if (p->token.text != "self")
            {
                CONCAT_FIXED_STR(bufferSwg, ": ");
                emitType(p->typeInfo, indent);
            }

            auto param = CastAst<AstVarDecl>(p, AstNodeKind::FuncDeclParam);
            if (param->assignment)
            {
                CONCAT_FIXED_STR(bufferSwg, " = ");
                SWAG_CHECK(Ast::output(outputContext, bufferSwg, param->assignment));
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
        emitType(typeFunc->returnType, indent);
    }

    if ((node->flags & AST_SHORT_LAMBDA) && !(node->returnType->flags & AST_FUNC_RETURN_DEFINED))
    {
        CONCAT_FIXED_STR(bufferSwg, " => ");
        SWAG_ASSERT(node->content->kind == AstNodeKind::Return);
        SWAG_CHECK(Ast::output(outputContext, bufferSwg, node->content->childs.front()));
    }
    else
    {
        bufferSwg.addEolIndent(indent);
        CONCAT_FIXED_STR(bufferSwg, "{");
        indent++;
        bufferSwg.addEol();

        if (node->content->kind != AstNodeKind::Statement)
        {
            bufferSwg.addIndent(outputContext.indent + 1);
            SWAG_CHECK(Ast::output(outputContext, bufferSwg, node->content));
            bufferSwg.addEol();
        }
        else
        {
            outputContext.indent = indent;

            for (auto c : node->subDecls)
            {
                bufferSwg.addIndent(outputContext.indent);
                SWAG_CHECK(Ast::output(outputContext, bufferSwg, c));
                bufferSwg.addEol();
            }

            for (auto c : node->content->childs)
            {
                bufferSwg.addIndent(outputContext.indent);
                SWAG_CHECK(Ast::output(outputContext, bufferSwg, c));
                bufferSwg.addEol();
            }
        }

        indent--;
        bufferSwg.addIndent(indent);
        CONCAT_FIXED_STR(bufferSwg, "}");
        bufferSwg.addEol();
    }

    bufferSwg.addEol();
    return true;
}

bool Backend::emitPublicEnumSwg(TypeInfoEnum* typeEnum, AstNode* node, int indent)
{
    SWAG_CHECK(emitAttributes(typeEnum, indent));
    bufferSwg.addIndent(indent);
    CONCAT_FIXED_STR(bufferSwg, "enum ");
    bufferSwg.addString(node->token.text.c_str());
    CONCAT_FIXED_STR(bufferSwg, " : ");
    emitType(typeEnum->rawType, indent);

    bufferSwg.addEolIndent(indent);
    CONCAT_FIXED_STR(bufferSwg, "{");
    bufferSwg.addEol();

    for (auto c : node->childs)
    {
        if (c->kind != AstNodeKind::EnumValue)
            continue;
        bufferSwg.addIndent(indent + 1);

        bufferSwg.addString(c->token.text);
        if (!c->childs.empty())
        {
            CONCAT_FIXED_STR(bufferSwg, " = ");
            SWAG_CHECK(Ast::output(outputContext, bufferSwg, c->childs.front()));
        }

        bufferSwg.addEol();
    }

    bufferSwg.addIndent(indent);
    CONCAT_FIXED_STR(bufferSwg, "}");
    bufferSwg.addEol();
    bufferSwg.addEol();
    return true;
}

bool Backend::emitPublicStructSwg(TypeInfoStruct* typeStruct, AstStruct* node, int indent)
{
    SWAG_CHECK(emitAttributes(typeStruct, indent));

    if (!(node->structFlags & STRUCTFLAG_ANONYMOUS))
        bufferSwg.addIndent(indent);

    if (node->kind == AstNodeKind::InterfaceDecl)
        CONCAT_FIXED_STR(bufferSwg, "interface");
    else if (node->kind == AstNodeKind::TypeSet)
        CONCAT_FIXED_STR(bufferSwg, "typeset");
    else
    {
        SWAG_ASSERT(node->kind == AstNodeKind::StructDecl)
        auto structNode = CastAst<AstStruct>(node, AstNodeKind::StructDecl);
        if (structNode->structFlags & STRUCTFLAG_UNION)
            CONCAT_FIXED_STR(bufferSwg, "union");
        else
            CONCAT_FIXED_STR(bufferSwg, "struct");
    }

    if (node->genericParameters)
        SWAG_CHECK(emitGenericParameters(node->genericParameters, indent));

    if (!(node->structFlags & STRUCTFLAG_ANONYMOUS))
    {
        CONCAT_FIXED_STR(bufferSwg, " ");
        bufferSwg.addString(node->token.text.c_str());
    }

    bufferSwg.addEol();
    bufferSwg.addIndent(indent);
    CONCAT_FIXED_STR(bufferSwg, "{");
    bufferSwg.addEol();

    for (auto p : typeStruct->fields)
    {
        SWAG_CHECK(emitAttributes(p, indent + 1));

        // Struct/interface content
        if (p->declNode->kind == AstNodeKind::VarDecl)
        {
            auto varDecl = CastAst<AstVarDecl>(p->declNode, AstNodeKind::VarDecl);
            SWAG_CHECK(emitVarSwg(nullptr, varDecl, indent + 1));
            bufferSwg.addEol();
        }

        // Typeset content
        else
        {
            SWAG_ASSERT(p->declNode->kind == AstNodeKind::StructDecl);
            SWAG_ASSERT(node->kind == AstNodeKind::TypeSet);
            bufferSwg.addIndent(indent + 1);
            bufferSwg.addString(p->namedParam);
            emitTypeTuple(p->typeInfo, indent);
            bufferSwg.addEol();
        }
    }

    for (auto p : typeStruct->consts)
    {
        auto varDecl = CastAst<AstVarDecl>(p->declNode, AstNodeKind::ConstDecl);
        SWAG_CHECK(emitAttributes(p, indent + 1));
        SWAG_CHECK(emitVarSwg("const ", varDecl, indent + 1));
        bufferSwg.addEol();
    }

    bufferSwg.addIndent(indent);
    CONCAT_FIXED_STR(bufferSwg, "}");

    if (!(node->structFlags & STRUCTFLAG_ANONYMOUS))
        bufferSwg.addEol();
    bufferSwg.addEol();

    return true;
}

bool Backend::emitVarSwg(const char* kindName, AstVarDecl* node, int indent)
{
    bufferSwg.addIndent(indent);
    if (node->flags & AST_DECL_USING)
        CONCAT_FIXED_STR(bufferSwg, "using ");
    if (kindName)
        bufferSwg.addString(kindName);

    if (!(node->flags & AST_AUTO_NAME))
    {
        bufferSwg.addString(node->token.text.c_str());
        if (node->type)
        {
            CONCAT_FIXED_STR(bufferSwg, ": ");
            emitType(node->typeInfo, indent);
        }
    }
    else
    {
        emitType(node->typeInfo, indent);
    }

    if (node->assignment)
    {
        CONCAT_FIXED_STR(bufferSwg, " = ");
        SWAG_CHECK(Ast::output(outputContext, bufferSwg, node->assignment));
    }

    return true;
}

bool Backend::emitPublicScopeContentSwg(Module* moduleToGen, Scope* scope, int indent)
{
    auto publicSet = scope->publicSet;
    if (!publicSet)
        return true;

    // Consts
    if (!publicSet->publicConst.empty())
    {
        for (auto one : publicSet->publicConst)
        {
            AstVarDecl* node = CastAst<AstVarDecl>(one, AstNodeKind::ConstDecl);
            SWAG_CHECK(emitVarSwg("const ", node, indent));
            bufferSwg.addEol();
        }
    }

    // Stuff (alias)
    if (!publicSet->publicNodes.empty())
    {
        for (auto one : publicSet->publicNodes)
        {
            bufferSwg.addIndent(indent);
            SWAG_CHECK(Ast::output(outputContext, bufferSwg, one));
            bufferSwg.addEol();
        }
    }

    // Structures
    if (!publicSet->publicStruct.empty())
    {
        for (auto one : publicSet->publicStruct)
        {
            AstStruct*      node       = CastAst<AstStruct>(one, AstNodeKind::StructDecl);
            TypeInfoStruct* typeStruct = CastTypeInfo<TypeInfoStruct>(node->typeInfo, TypeInfoKind::Struct);
            SWAG_CHECK(emitPublicStructSwg(typeStruct, node, indent));
        }
    }

    if (!publicSet->publicInterface.empty())
    {
        for (auto one : publicSet->publicInterface)
        {
            AstStruct*      node       = CastAst<AstStruct>(one, AstNodeKind::InterfaceDecl);
            TypeInfoStruct* typeStruct = CastTypeInfo<TypeInfoStruct>(node->typeInfo, TypeInfoKind::Interface);
            SWAG_CHECK(emitPublicStructSwg(typeStruct->itable, node, indent));
        }
    }

    if (!publicSet->publicTypeSet.empty())
    {
        for (auto one : publicSet->publicTypeSet)
        {
            AstStruct*      node       = CastAst<AstStruct>(one, AstNodeKind::TypeSet);
            TypeInfoStruct* typeStruct = CastTypeInfo<TypeInfoStruct>(node->typeInfo, TypeInfoKind::TypeSet);
            SWAG_CHECK(emitPublicStructSwg(typeStruct, node, indent));
        }
    }

    // Enums
    if (!publicSet->publicEnum.empty())
    {
        for (auto one : publicSet->publicEnum)
        {
            TypeInfoEnum* typeEnum = CastTypeInfo<TypeInfoEnum>(one->typeInfo, TypeInfoKind::Enum);
            SWAG_CHECK(emitPublicEnumSwg(typeEnum, one, indent));
        }
    }

    // Generic functions
    if (!publicSet->publicInlinedFunc.empty())
    {
        for (auto func : publicSet->publicInlinedFunc)
        {
            AstFuncDecl*      node     = CastAst<AstFuncDecl>(func, AstNodeKind::FuncDecl);
            TypeInfoFuncAttr* typeFunc = CastTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FuncAttr);
            SWAG_CHECK(emitPublicFuncSwg(typeFunc, node, indent));
        }
    }

    // Functions
    if (!publicSet->publicFunc.empty())
    {
        for (auto func : publicSet->publicFunc)
        {
            AstFuncDecl* node = CastAst<AstFuncDecl>(func, AstNodeKind::FuncDecl);
            if (node->isSpecialFunctionGenerated())
                continue;
            TypeInfoFuncAttr* typeFunc = CastTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FuncAttr);
            node->computeFullNameForeign(true);
            bufferSwg.addIndent(indent);

            // Remape special functions to their generated equivalent
            auto nf = node->fullnameForeign;
            if (node->token.text == "opInit" ||
                node->token.text == "opDrop" ||
                node->token.text == "opReloc" ||
                node->token.text == "opPostCopy" ||
                node->token.text == "opPostMove")
                nf += "Generated";

            bufferSwg.addStringFormat("#[foreign(\"%s\", \"%s\")]", module->name.c_str(), nf.c_str());
            bufferSwg.addEol();
            SWAG_CHECK(emitAttributes(typeFunc, indent));
            bufferSwg.addIndent(indent);
            SWAG_CHECK(emitFuncSignatureSwg(typeFunc, node, indent));
            node->exportForeignLine = bufferSwg.eolCount;
        }
    }

    // Attributes
    if (!publicSet->publicAttr.empty())
    {
        for (auto func : publicSet->publicAttr)
        {
            auto              node     = CastAst<AstAttrDecl>(func, AstNodeKind::AttrDecl);
            TypeInfoFuncAttr* typeFunc = CastTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FuncAttr);
            emitAttributesUsage(typeFunc, indent);
            bufferSwg.addIndent(indent);
            SWAG_CHECK(emitFuncSignatureSwg(typeFunc, node, node->parameters, nullptr, indent));
        }
    }

    return true;
}

bool Backend::emitPublicScopeSwg(Module* moduleToGen, Scope* scope, int indent)
{
    SWAG_ASSERT(moduleToGen);
    if (!(scope->flags & SCOPE_FLAG_HAS_EXPORTS))
        return true;
    if (scope->flags & SCOPE_IMPORTED)
        return true;

    outputContext.forExport = true;

    // Namespace
    if (scope->kind == ScopeKind::Namespace && !scope->name.empty())
    {
        if (!(scope->flags & SCOPE_AUTO_GENERATED))
        {
            bufferSwg.addIndent(indent);
            bufferSwg.addStringFormat("namespace %s", scope->name.c_str());
            bufferSwg.addEol();
            bufferSwg.addIndent(indent);
            CONCAT_FIXED_STR(bufferSwg, "{");
            bufferSwg.addEol();
            indent += 1;
        }

        SWAG_CHECK(emitPublicScopeContentSwg(moduleToGen, scope, indent));
        for (auto oneScope : scope->childScopes)
            SWAG_CHECK(emitPublicScopeSwg(moduleToGen, oneScope, indent));

        if (!(scope->flags & SCOPE_AUTO_GENERATED))
        {
            indent -= 1;
            bufferSwg.addIndent(indent);
            CONCAT_FIXED_STR(bufferSwg, "}");
            bufferSwg.addEol();
            bufferSwg.addEol();
        }
    }

    // Impl
    else if (!scope->isGlobal() && scope->isGlobalOrImpl() && !scope->name.empty())
    {
        bufferSwg.addIndent(indent);
        if (scope->kind == ScopeKind::Impl)
        {
            auto nodeImpl = CastAst<AstImpl>(scope->owner, AstNodeKind::Impl);
            auto symbol   = nodeImpl->identifier->resolvedSymbolOverload;
            bufferSwg.addStringFormat("impl %s for %s", symbol->node->getScopedName().c_str(), scope->parentScope->name.c_str());
            bufferSwg.addEol();
        }
        else if (scope->kind == ScopeKind::Enum)
        {
            bufferSwg.addStringFormat("impl enum %s", scope->name.c_str());
            bufferSwg.addEol();
        }
        else
        {
            bufferSwg.addStringFormat("impl %s", scope->name.c_str());
            bufferSwg.addEol();
        }

        bufferSwg.addIndent(indent);
        CONCAT_FIXED_STR(bufferSwg, "{");
        bufferSwg.addEol();

        SWAG_CHECK(emitPublicScopeContentSwg(moduleToGen, scope, indent + 1));
        for (auto oneScope : scope->childScopes)
        {
            if (oneScope->kind == ScopeKind::Impl)
                continue;
            SWAG_CHECK(emitPublicScopeSwg(moduleToGen, oneScope, indent + 1));
        }

        bufferSwg.addIndent(indent);
        CONCAT_FIXED_STR(bufferSwg, "}");
        bufferSwg.addEol();
        bufferSwg.addEol();

        for (auto oneScope : scope->childScopes)
        {
            if (oneScope->kind != ScopeKind::Impl)
                continue;
            SWAG_CHECK(emitPublicScopeSwg(moduleToGen, oneScope, indent));
        }
    }

    // Named scope
    else if (!scope->name.empty())
    {
        bufferSwg.addIndent(indent);
        CONCAT_FIXED_STR(bufferSwg, "{");
        bufferSwg.addEol();

        SWAG_CHECK(emitPublicScopeContentSwg(moduleToGen, scope, indent + 1));
        for (auto oneScope : scope->childScopes)
            SWAG_CHECK(emitPublicScopeSwg(moduleToGen, oneScope, indent + 1));

        bufferSwg.addIndent(indent);
        CONCAT_FIXED_STR(bufferSwg, "}");
        bufferSwg.addEol();
        bufferSwg.addEol();
    }

    // Unnamed scope
    else
    {
        SWAG_CHECK(emitPublicScopeContentSwg(moduleToGen, scope, indent));
        for (auto oneScope : scope->childScopes)
            SWAG_CHECK(emitPublicScopeSwg(moduleToGen, oneScope, indent));
    }

    return true;
}

void Backend::emitDependencies()
{
    for (const auto& dep : module->moduleDependencies)
    {
        bufferSwg.addStringFormat("#import \"%s\"", dep->name.c_str());
        bufferSwg.addEol();
    }
}

bool Backend::setupExportFile(bool force)
{
    if (!bufferSwg.path.empty())
        return true;

    Utf8 publicPath = g_Workspace.getPublicPath(module, true);
    if (publicPath.empty())
        return false;

    Utf8 exportName = module->name + ".swg";
    publicPath.append(exportName.c_str());
    if (force)
    {
        bufferSwg.name = exportName;
        bufferSwg.path = publicPath;
    }
    else if (fs::exists(publicPath.c_str()))
    {
        bufferSwg.name = exportName;
        bufferSwg.path = publicPath;
        timeExportFile = OS::getFileWriteTime(publicPath.c_str());
    }

    return true;
}

JobResult Backend::generateExportFile(Job* ownerJob)
{
    SWAG_PROFILE(PRF_OUT, format("generateExportFile %s", module->name.c_str()));
    if (passExport == BackendPreCompilePass::Init)
    {
        passExport = BackendPreCompilePass::GenerateObj;
        if (!setupExportFile(true))
            return JobResult::ReleaseJob;
        if (!mustCompile)
            return JobResult::ReleaseJob;

        bufferSwg.init(4 * 1024);
        bufferSwg.addStringFormat("// GENERATED WITH SWAG VERSION %d.%d.%d", SWAG_BUILD_VERSION, SWAG_BUILD_REVISION, SWAG_BUILD_NUM);
        bufferSwg.addEol();
        bufferSwg.addString("#global generated");
        module->isSwag = true;
        bufferSwg.addEol();

        emitDependencies();

        CONCAT_FIXED_STR(bufferSwg, "using swag");
        bufferSwg.addEol();
        bufferSwg.addEol();

        // Emit everything that's public
        if (!emitPublicScopeSwg(module, module->scopeRoot, 0))
            return JobResult::ReleaseJob;
    }

    // Save the export file
    if (passExport == BackendPreCompilePass::GenerateObj)
    {
        passExport        = BackendPreCompilePass::Release;
        auto job          = g_Pool_moduleSaveExportJob.alloc();
        job->module       = module;
        job->dependentJob = ownerJob;
        ownerJob->jobsToAdd.push_back(job);
        return JobResult::KeepJobAlive;
    }

    return JobResult::ReleaseJob;
}

bool Backend::saveExportFile()
{
    SWAG_PROFILE(PRF_SAVE, format("saveExportFile %s", module->name.c_str()));
    auto result = bufferSwg.flush(true, AFFINITY_ALL ^ AFFINITY_IO);
    if (!result)
        return false;
    timeExportFile = OS::getFileWriteTime(bufferSwg.path.c_str());
    SWAG_ASSERT(timeExportFile);
    module->setHasBeenBuilt(BUILDRES_EXPORT);
    return true;
}