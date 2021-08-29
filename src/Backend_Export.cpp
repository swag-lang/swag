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

bool Backend::emitPublicScopeSwg(Module* moduleToGen, Scope* scope, int indent)
{
    outputContext.indent = indent;
    return Ast::outputScope(outputContext, bufferSwg, moduleToGen, scope);
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