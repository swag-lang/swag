#include "pch.h"
#include "Backend.h"
#include "Workspace.h"
#include "Version.h"
#include "TypeManager.h"
#include "Os.h"
#include "Module.h"
#include "ModuleSaveExportJob.h"
#include "AstNode.h"
#include "ByteCode.h"
#include "LanguageSpec.h"

bool Backend::emitAttributesUsage(TypeInfoFuncAttr* typeFunc, int indent)
{
    outputContext.indent = indent;
    return Ast::outputAttributesUsage(outputContext, bufferSwg, typeFunc);
}

bool Backend::emitAttributes(TypeInfo* typeInfo, int indent)
{
    outputContext.indent = indent;
    return Ast::outputAttributes(outputContext, bufferSwg, typeInfo);
}

bool Backend::emitType(TypeInfo* typeInfo, int indent)
{
    outputContext.indent = indent;
    return Ast::outputType(outputContext, bufferSwg, typeInfo);
}

bool Backend::emitGenericParameters(AstNode* node, int indent)
{
    outputContext.indent = indent;
    return Ast::outputGenericParameters(outputContext, bufferSwg, node);
}

bool Backend::emitFuncSignatureSwg(TypeInfoFuncAttr* typeFunc, AstFuncDecl* node, int indent)
{
    return emitFuncSignatureSwg(typeFunc, node, node->parameters, node->selectIf, indent);
}

bool Backend::emitFuncSignatureSwg(TypeInfoFuncAttr* typeFunc, AstNode* node, AstNode* parameters, AstNode* selectIf, int indent)
{
    outputContext.indent = indent;
    return Ast::outputFuncSignature(outputContext, bufferSwg, typeFunc, node, parameters, selectIf);
}

bool Backend::emitPublicFuncSwg(TypeInfoFuncAttr* typeFunc, AstFuncDecl* node, int indent)
{
    outputContext.indent = indent;
    return Ast::outputFunc(outputContext, bufferSwg, typeFunc, node);
}

bool Backend::emitPublicEnumSwg(TypeInfoEnum* typeEnum, AstNode* node, int indent)
{
    outputContext.indent = indent;
    return Ast::outputEnum(outputContext, bufferSwg, typeEnum, node);
}

bool Backend::emitPublicStructSwg(TypeInfoStruct* typeStruct, AstStruct* node, int indent)
{
    outputContext.indent = indent;
    return Ast::outputStruct(outputContext, bufferSwg, typeStruct, node);
}

bool Backend::emitVarSwg(const char* kindName, AstVarDecl* node, int indent)
{
    outputContext.indent = indent;
    return Ast::outputVar(outputContext, bufferSwg, kindName, node);
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

            // Can be removed in case of special functions
            if (!(node->attributeFlags & ATTRIBUTE_PUBLIC))
                continue;

            TypeInfoFuncAttr* typeFunc = CastTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FuncAttr);
            node->computeFullNameForeign(true);
            bufferSwg.addIndent(indent);

            // Remape special functions to their generated equivalent
            bufferSwg.addStringFormat("#[Foreign(\"%s\", \"%s\")]", module->name.c_str(), node->fullnameForeign.c_str());
            bufferSwg.addEol();
            SWAG_CHECK(emitAttributes(typeFunc, indent));
            bufferSwg.addIndent(indent);

            if (node->token.text == g_LangSpec->name_opInitGenerated)
            {
                CONCAT_FIXED_STR(bufferSwg, "func opInit(using self);");
                bufferSwg.addEol();
            }
            else if (node->token.text == g_LangSpec->name_opDropGenerated)
            {
                CONCAT_FIXED_STR(bufferSwg, "func opDrop(using self);");
                bufferSwg.addEol();
            }
            else if (node->token.text == g_LangSpec->name_opRelocGenerated)
            {
                CONCAT_FIXED_STR(bufferSwg, "func opReloc(using self);");
                bufferSwg.addEol();
            }
            else if (node->token.text == g_LangSpec->name_opPostCopyGenerated)
            {
                CONCAT_FIXED_STR(bufferSwg, "func opPostCopy(using self);");
                bufferSwg.addEol();
            }
            else if (node->token.text == g_LangSpec->name_opPostMoveGenerated)
            {
                CONCAT_FIXED_STR(bufferSwg, "func opPostMove(using self);");
                bufferSwg.addEol();
            }
            else
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
            CONCAT_FIXED_STR(bufferSwg, "namespace ");
            bufferSwg.addString(scope->name);
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
            CONCAT_FIXED_STR(bufferSwg, "impl enum ");
            bufferSwg.addString(scope->name);
            bufferSwg.addEol();
        }
        else
        {
            CONCAT_FIXED_STR(bufferSwg, "impl ");
            bufferSwg.addString(scope->name);
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
        CONCAT_FIXED_STR(bufferSwg, "#import \"");
        bufferSwg.addString(dep->name);
        bufferSwg.addChar('"');
        bufferSwg.addEol();
    }
}

bool Backend::setupExportFile(bool force)
{
    if (!exportFilePath.empty())
        return true;

    auto publicPath = module->publicPath;
    if (publicPath.empty())
        return false;

    Utf8 exportName = module->name + ".swg";
    publicPath.append(exportName.c_str());
    if (force)
    {
        exportFileName = exportName;
        exportFilePath = publicPath;
    }
    else
    {
        exportFileName = exportName;
        exportFilePath = publicPath;
        timeExportFile = OS::getFileWriteTime(publicPath.c_str());
    }

    return true;
}

JobResult Backend::generateExportFile(Job* ownerJob)
{
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

        CONCAT_FIXED_STR(bufferSwg, "using Swag");
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
        auto job          = g_Allocator.alloc<ModuleSaveExportJob>();
        job->module       = module;
        job->dependentJob = ownerJob;
        ownerJob->jobsToAdd.push_back(job);
        return JobResult::KeepJobAlive;
    }

    return JobResult::ReleaseJob;
}

bool Backend::saveExportFile()
{
    auto result = bufferSwg.flushToFile(exportFilePath);
    if (!result)
        return false;
    timeExportFile = OS::getFileWriteTime(exportFilePath.c_str());
    SWAG_ASSERT(timeExportFile);
    module->setHasBeenBuilt(BUILDRES_EXPORT);
    return true;
}