#include "pch.h"
#include "Backend.h"
#include "Workspace.h"
#include "Version.h"
#include "Ast.h"
#include "TypeManager.h"
#include "Os.h"
#include "Module.h"
#include "Profile.h"
#include "ModuleSaveExportJob.h"

bool Backend::emitAttributes(AstNode* node, int indent)
{
#define ADD_ATTR(__test, __name)                   \
    {                                              \
        if (__test)                                \
        {                                          \
            if (first)                             \
            {                                      \
                bufferSwg.addIndent(indent);       \
                CONCAT_FIXED_STR(bufferSwg, "#["); \
                first = false;                     \
            }                                      \
            else                                   \
            {                                      \
                CONCAT_FIXED_STR(bufferSwg, ", "); \
            }                                      \
            CONCAT_FIXED_STR(bufferSwg, __name);   \
        }                                          \
    }

    bool first = true;
    ADD_ATTR(node->flags & AST_CONST_EXPR, "constexpr");
    ADD_ATTR(node->attributeFlags & ATTRIBUTE_MACRO, "macro");
    ADD_ATTR((node->attributeFlags & ATTRIBUTE_MIXIN) && !(node->attributeFlags & ATTRIBUTE_MACRO), "mixin");
    ADD_ATTR((node->attributeFlags & ATTRIBUTE_INLINE) && !(node->attributeFlags & ATTRIBUTE_MIXIN) && !(node->attributeFlags & ATTRIBUTE_MACRO), "inline");
    ADD_ATTR(node->attributeFlags & ATTRIBUTE_PRINTBYTECODE, "printbc");
    ADD_ATTR(node->attributeFlags & ATTRIBUTE_NORETURN, "noreturn");
    ADD_ATTR(node->attributeFlags & ATTRIBUTE_COMPLETE, "complete");
    ADD_ATTR(node->attributeFlags & ATTRIBUTE_PROPERTY, "property");
    if (!first)
        CONCAT_FIXED_STR(bufferSwg, "]\n");
    return true;
}

bool Backend::emitAttributes(TypeInfoParam* param, int indent)
{
    ComputedValue value;
    if (param->attributes.getValue("swag.offset", "name", value))
    {
        bufferSwg.addIndent(indent);
        bufferSwg.addStringFormat("#[offset(\"%s\")]\n", value.text.c_str());
    }

    return true;
}

void Backend::emitType(TypeInfo* typeInfo)
{
    if (typeInfo->kind == TypeInfoKind::Lambda)
    {
        auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(typeInfo, TypeInfoKind::Lambda);
        bufferSwg.addString("func(");
        for (auto p : typeFunc->parameters)
        {
            if (p != typeFunc->parameters[0])
                bufferSwg.addString(", ");
            emitType(p->typeInfo);
        }

        bufferSwg.addString(")->");
        emitType(typeFunc->returnType);
    }
    else
    {
        // Be sure to keep the original baked name
        if (typeInfo->kind == TypeInfoKind::Reference)
        {
            auto typeRef = CastTypeInfo<TypeInfoReference>(typeInfo, TypeInfoKind::Reference);
            if (typeRef->originalType->flags & TYPEINFO_BAKE)
                typeInfo = typeRef->originalType;
        }

        if (typeInfo->flags & TYPEINFO_SELF)
        {
            if (typeInfo->flags & TYPEINFO_CONST)
                bufferSwg.addString("const self");
            else
                bufferSwg.addString("self");
        }
        else
        {
            typeInfo = TypeManager::concreteReference(typeInfo);
            typeInfo->computeScopedName();
            bufferSwg.addString(typeInfo->scopedName);
        }
    }
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
            SWAG_CHECK(Ast::output(outputContext, bufferSwg, varDecl->type));
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

bool Backend::emitFuncSignatureSwg(TypeInfoFuncAttr* typeFunc, AstFuncDecl* node)
{
    CONCAT_FIXED_STR(bufferSwg, "func ");
    bufferSwg.addString(node->name.c_str());
    CONCAT_FIXED_STR(bufferSwg, "(");

    if (node->parameters)
    {
        uint32_t idx = 0;
        for (auto p : typeFunc->parameters)
        {
            AstVarDecl* varDecl = CastAst<AstVarDecl>(node->parameters->childs[idx], AstNodeKind::VarDecl, AstNodeKind::FuncDeclParam);

            // Name
            if (varDecl->name == "self" && p->typeInfo->isConst())
                bufferSwg.addString("const ");

            bufferSwg.addString(varDecl->name);

            // Type
            if (varDecl->name != "self")
            {
                CONCAT_FIXED_STR(bufferSwg, ": ");
                emitType(p->typeInfo);
            }

            // Assignment
            if (varDecl->assignment)
            {
                CONCAT_FIXED_STR(bufferSwg, " = ");
                Ast::output(outputContext, bufferSwg, varDecl->assignment);
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
        emitType(typeFunc->returnType);
    }

    CONCAT_FIXED_STR(bufferSwg, ";");
    CONCAT_FIXED_STR(bufferSwg, "\n");
    return true;
}

bool Backend::emitPublicFuncSwg(TypeInfoFuncAttr* typeFunc, AstFuncDecl* node, int indent)
{
    SWAG_CHECK(emitAttributes(node, indent));
    bufferSwg.addIndent(indent);
    CONCAT_FIXED_STR(bufferSwg, "func");

    // Emit generic parameter, except if the function is an instance
    if (node->genericParameters && !(node->flags & AST_FROM_GENERIC))
    {
        SWAG_CHECK(emitGenericParameters(node->genericParameters));
    }

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

            if (p->name == "self" && p->typeInfo->isConst())
                bufferSwg.addString("const ");

            bufferSwg.addString(p->name);

            if (p->name != "self")
            {
                CONCAT_FIXED_STR(bufferSwg, ": ");
                emitType(p->typeInfo);
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
        emitType(typeFunc->returnType);
    }

    if (node->content->kind != AstNodeKind::Statement)
    {
        bufferSwg.addEolIndent(indent);
        CONCAT_FIXED_STR(bufferSwg, "{");
        indent++;
    }

    bufferSwg.addEolIndent(indent);
    outputContext.indent = indent;
    Ast::output(outputContext, bufferSwg, node->content);

    if (node->content->kind != AstNodeKind::Statement)
    {
        bufferSwg.addEol();
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
    bufferSwg.addIndent(indent);
    CONCAT_FIXED_STR(bufferSwg, "enum ");
    bufferSwg.addString(node->name.c_str());
    CONCAT_FIXED_STR(bufferSwg, " : ");
    bufferSwg.addString(typeEnum->rawType->name);

    bufferSwg.addEolIndent(indent);
    CONCAT_FIXED_STR(bufferSwg, "{");
    bufferSwg.addEol();

    for (auto p : typeEnum->values)
    {
        bufferSwg.addIndent(indent + 1);
        bufferSwg.addString(p->namedParam);
        CONCAT_FIXED_STR(bufferSwg, " = ");
        SWAG_CHECK(Ast::outputLiteral(outputContext, bufferSwg, node, typeEnum->rawType, p->value.text, p->value.reg));
        bufferSwg.addEol();
    }

    bufferSwg.addIndent(indent);
    CONCAT_FIXED_STR(bufferSwg, "}");
    bufferSwg.addEol();
    bufferSwg.addEol();
    return true;
}

bool Backend::emitPublicConstSwg(AstVarDecl* node, int indent)
{
    bufferSwg.addIndent(indent);
    CONCAT_FIXED_STR(bufferSwg, "const ");
    bufferSwg.addString(node->name.c_str());

    if (node->type)
    {
        CONCAT_FIXED_STR(bufferSwg, ": ");
        SWAG_CHECK(Ast::output(outputContext, bufferSwg, node->type));
    }

    if (node->assignment)
    {
        CONCAT_FIXED_STR(bufferSwg, " = ");
        SWAG_CHECK(Ast::outputLiteral(outputContext, bufferSwg, node->assignment, node->assignment->typeInfo, node->computedValue.text, node->computedValue.reg));
    }

    bufferSwg.addEol();
    return true;
}

bool Backend::emitPublicStructSwg(TypeInfoStruct* typeStruct, AstStruct* node, int indent)
{
    bufferSwg.addIndent(indent);
    if (node->kind == AstNodeKind::InterfaceDecl)
        CONCAT_FIXED_STR(bufferSwg, "interface");
    else
        CONCAT_FIXED_STR(bufferSwg, "struct");

    // If the node comes from a batch, then there's no more generic parameters
    if (node->genericParameters && !(node->flags & AST_FROM_BAKE))
        SWAG_CHECK(emitGenericParameters(node->genericParameters));

    CONCAT_FIXED_STR(bufferSwg, " ");
    if (node->flags & AST_FROM_BAKE)
        bufferSwg.addString(node->bakeName.c_str());
    else
        bufferSwg.addString(node->name.c_str());
    CONCAT_FIXED_STR(bufferSwg, "\n");
    bufferSwg.addIndent(indent);
    CONCAT_FIXED_STR(bufferSwg, "{\n");

    for (auto p : typeStruct->fields)
    {
        SWAG_CHECK(emitAttributes(p, indent + 1));
        bufferSwg.addIndent(indent + 1);
        if (p->node->attributeFlags & ATTRIBUTE_READWRITE)
            CONCAT_FIXED_STR(bufferSwg, "readwrite ");
        else if (p->node->attributeFlags & ATTRIBUTE_INTERNAL)
            CONCAT_FIXED_STR(bufferSwg, "internal ");
        else if (p->node->attributeFlags & ATTRIBUTE_READONLY)
            CONCAT_FIXED_STR(bufferSwg, "readonly ");
        bufferSwg.addString(p->namedParam);
        CONCAT_FIXED_STR(bufferSwg, ": ");
        emitType(p->typeInfo);

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
                SWAG_CHECK(Ast::outputLiteral(outputContext, bufferSwg, node, p->typeInfo, p->value.text, p->value.reg));
            }
        }

        CONCAT_FIXED_STR(bufferSwg, "\n");
    }

    bufferSwg.addIndent(indent);
    CONCAT_FIXED_STR(bufferSwg, "}\n\n");
    return true;
}

bool Backend::emitPublicScopeContentSwg(Module* moduleToGen, Scope* scope, int indent)
{
    // Consts
    if (!scope->publicConst.empty())
    {
        for (auto one : scope->publicConst)
        {
            AstVarDecl* node = CastAst<AstVarDecl>(one, AstNodeKind::ConstDecl);
            SWAG_CHECK(emitPublicConstSwg(node, indent));
        }
    }

    // Stuff (alias, using will go there)
    if (!scope->publicNodes.empty())
    {
        for (auto one : scope->publicNodes)
        {
            bufferSwg.addIndent(indent);
            SWAG_CHECK(Ast::output(outputContext, bufferSwg, one));
            bufferSwg.addEol();
        }
    }

    // Structures
    if (!scope->publicStruct.empty())
    {
        for (auto one : scope->publicStruct)
        {
            AstStruct*      node       = CastAst<AstStruct>(one, AstNodeKind::StructDecl);
            TypeInfoStruct* typeStruct = CastTypeInfo<TypeInfoStruct>(node->typeInfo, TypeInfoKind::Struct);
            SWAG_CHECK(emitPublicStructSwg(typeStruct, node, indent));
        }
    }

    if (!scope->publicInterface.empty())
    {
        for (auto one : scope->publicInterface)
        {
            AstStruct*      node       = CastAst<AstStruct>(one, AstNodeKind::InterfaceDecl);
            TypeInfoStruct* typeStruct = CastTypeInfo<TypeInfoStruct>(node->typeInfo, TypeInfoKind::Interface);
            SWAG_CHECK(emitPublicStructSwg(typeStruct->itable, node, indent));
        }
    }

    // Enums
    if (!scope->publicEnum.empty())
    {
        for (auto one : scope->publicEnum)
        {
            TypeInfoEnum* typeEnum = CastTypeInfo<TypeInfoEnum>(one->typeInfo, TypeInfoKind::Enum);
            SWAG_CHECK(emitPublicEnumSwg(typeEnum, one, indent));
        }
    }

    // Generic functions
    if (!scope->publicGenericFunc.empty())
    {
        for (auto func : scope->publicGenericFunc)
        {
            AstFuncDecl*      node     = CastAst<AstFuncDecl>(func, AstNodeKind::FuncDecl);
            TypeInfoFuncAttr* typeFunc = CastTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FuncAttr);
            SWAG_CHECK(emitPublicFuncSwg(typeFunc, node, indent));
        }
    }

    // Functions
    if (!scope->publicFunc.empty())
    {
        for (auto func : scope->publicFunc)
        {
            AstFuncDecl*      node     = CastAst<AstFuncDecl>(func, AstNodeKind::FuncDecl);
            TypeInfoFuncAttr* typeFunc = CastTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FuncAttr);
            node->computeFullNameForeign(true);
            bufferSwg.addIndent(indent);
            bufferSwg.addStringFormat("#[foreign(\"%s\", \"%s\")]\n", module->name.c_str(), node->fullnameForeign.c_str());
            SWAG_CHECK(emitAttributes(node, indent));
            bufferSwg.addIndent(indent);
            SWAG_CHECK(emitFuncSignatureSwg(typeFunc, node));
        }
    }

    for (auto oneScope : scope->childScopes)
        SWAG_CHECK(emitPublicScopeSwg(moduleToGen, oneScope, indent));

    return true;
}

bool Backend::emitPublicScopeSwg(Module* moduleToGen, Scope* scope, int indent)
{
    SWAG_ASSERT(moduleToGen);
    if (!(scope->flags & SCOPE_FLAG_HAS_EXPORTS))
        return true;

    outputContext.forExport = true;

    // Scope name
    if (!scope->name.empty())
    {
        if (scope->kind == ScopeKind::Namespace)
        {
            bufferSwg.addIndent(indent);
            bufferSwg.addStringFormat("namespace %s\n", scope->name.c_str());
            bufferSwg.addIndent(indent);
            CONCAT_FIXED_STR(bufferSwg, "{\n");
            indent++;
        }
        else if (!scope->isGlobal() && scope->isGlobalOrImpl())
        {
            bufferSwg.addIndent(indent);
            bufferSwg.addStringFormat("impl %s\n", scope->name.c_str());
            bufferSwg.addIndent(indent);
            CONCAT_FIXED_STR(bufferSwg, "{\n");
            indent++;
        }
        else
        {
            bufferSwg.addIndent(indent);
            CONCAT_FIXED_STR(bufferSwg, "{\n");
            indent++;
        }
    }

    SWAG_CHECK(emitPublicScopeContentSwg(moduleToGen, scope, indent));

    // End of named scope
    if (!scope->name.empty())
    {
        indent--;
        bufferSwg.addIndent(indent);
        CONCAT_FIXED_STR(bufferSwg, "}");
        bufferSwg.addEol();
        bufferSwg.addEol();
    }

    return true;
}

void Backend::setupExportFile()
{
    if (bufferSwg.path.empty())
    {
        exportFileGenerated = true;
        Utf8 targetName     = module->name + ".import.swg";
        auto targetPath     = g_Workspace.cachePath;
        targetPath.append(targetName.c_str());
        bool exists = fs::exists(targetPath.c_str());
        if (!exists)
        {
            exportFileGenerated = false;
            targetName          = module->name + ".swg";
            targetPath          = g_Workspace.targetPath.c_str();
            auto otherName      = module->name + ".swg";
            targetPath.append(otherName.c_str());
            exists = fs::exists(targetPath.c_str());
        }

        if (exists)
        {
            bufferSwg.name = targetName;
            bufferSwg.path = normalizePath(fs::path(targetPath.string().c_str()));
            timeExportFile = OS::getFileWriteTime(targetPath.string().c_str());
        }
    }
    else
    {
        SWAG_ASSERT(timeExportFile);
    }
}

JobResult Backend::generateExportFile(Job* ownerJob)
{
    SWAG_PROFILE(PRF_OUT, format("generateExportFile %s", module->name.c_str()));
    if (passExport == BackendPreCompilePass::Init)
    {
        passExport          = BackendPreCompilePass::GenerateObj;
        exportFileGenerated = true;
        bufferSwg.name      = module->name + ".import.swg";
        auto targetPath     = g_Workspace.cachePath;
        targetPath.append(bufferSwg.name.c_str());
        bufferSwg.path = normalizePath(targetPath);
        if (!mustCompile)
            return JobResult::ReleaseJob;

        bufferSwg.init(4 * 1024);
        bufferSwg.addStringFormat("// GENERATED WITH SWAG VERSION %d.%d.%d\n", SWAG_BUILD_VERSION, SWAG_BUILD_REVISION, SWAG_BUILD_NUM);

        for (const auto& dep : module->moduleDependencies)
            bufferSwg.addStringFormat("#import \"%s\"\n", dep->name.c_str());
        CONCAT_FIXED_STR(bufferSwg, "using swag\n");

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