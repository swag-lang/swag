#include "pch.h"
#include "BackendX64.h"
#include "Workspace.h"
#include "OS.h"
#include "Module.h"

bool BackendX64::createRuntime(const BuildParameters& buildParameters)
{
    return true;
}

JobResult BackendX64::preCompile(const BuildParameters& buildParameters, Job* ownerJob)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = perThread[ct][precompileIndex];
    auto& concat          = pp.concat;

    // Message
    if (pp.pass == BackendPreCompilePass::Init && buildParameters.precompileIndex == 0)
        module->printUserMessage(buildParameters);

    // Do we need to generate the file ?
    if (!mustCompile)
        return JobResult::ReleaseJob;

    if (pp.pass == BackendPreCompilePass::Init)
    {
        pp.pass = BackendPreCompilePass::FunctionBodies;

        pp.filename = format("%s%d", buildParameters.outputFileName.c_str(), precompileIndex);
        pp.filename += buildParameters.postFix;
        pp.filename += ".obj";

        if (g_CommandLine.verbose)
            g_Log.verbose(format("   module %s, x64 backend, precompile", perThread[ct][precompileIndex].filename.c_str(), module->byteCodeTestFunc.size()));

        emitHeader(buildParameters);
        createRuntime(buildParameters);
        emitDataSegment(buildParameters, &module->bssSegment);
        emitDataSegment(buildParameters, &module->mutableSegment);
        emitDataSegment(buildParameters, &module->constantSegment);
    }

    if (pp.pass == BackendPreCompilePass::FunctionBodies)
    {
        pp.pass              = BackendPreCompilePass::End;
        pp.textSectionOffset = concat.totalCount;
        applyPatch(pp, PatchType::TextSectionOffset, pp.textSectionOffset);
        emitAllFunctionBody(buildParameters, module, ownerJob);
        return JobResult::KeepJobAlive;
    }

    if (pp.pass == BackendPreCompilePass::End)
    {
        if (precompileIndex == 0)
        {
            emitInitDataSeg(buildParameters);
            emitInitConstantSeg(buildParameters);
            emitGlobalInit(buildParameters);
            emitGlobalDrop(buildParameters);
            emitMain(buildParameters);
        }

        // Output file
        emitSymbolTable(buildParameters);
        emitStringTable(buildParameters);
        generateObjFile(buildParameters);
    }

    return JobResult::ReleaseJob;
}

void BackendX64::addPatch(X64PerThread& pp, PatchType type, void* addr)
{
    pp.allPatches[type] = addr;
}

void BackendX64::applyPatch(X64PerThread& pp, PatchType type, uint32_t value)
{
    auto it = pp.allPatches.find(type);
    SWAG_ASSERT(it != pp.allPatches.end());
    *(uint32_t*) it->second = value;
}

bool BackendX64::emitHeader(const BuildParameters& buildParameters)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = perThread[ct][precompileIndex];
    auto& concat          = pp.concat;

    // Coff header
    /////////////////////////////////////////////
    concat.addU16(IMAGE_FILE_MACHINE_AMD64); // .Machine
    concat.addU16(4);                        // .NumberOfSections

    time_t now;
    time(&now);
    concat.addU32((uint32_t)(now & 0xFFFFFFFF)); // .TimeDateStamp

    addPatch(pp, PatchType::SymbolTableOffset, concat.addU32Addr(0)); // .PointerToSymbolTable
    addPatch(pp, PatchType::SymbolTableCount, concat.addU32Addr(0));  // .NumberOfSymbols

    concat.addU16(0); // .SizeOfOptionalHeader

    concat.addU16(IMAGE_FILE_LARGE_ADDRESS_AWARE | IMAGE_FILE_DEBUG_STRIPPED); // .Characteristics

    // bss section
    /////////////////////////////////////////////
    concat.addString(".bss", 8); // .Name
    concat.addU32(0);            // .VirtualSize
    concat.addU32(0);            // .VirtualAddress
    concat.addU32(0);            // .SizeOfRawData
    concat.addU32(0);            // .PointerToRawData
    concat.addU32(0);            // .PointerToRelocations
    concat.addU32(0);            // .PointerToLinenumbers
    concat.addU16(0);            // .NumberOfRelocations
    concat.addU16(0);            // .NumberOfLinenumbers
    concat.addU32(IMAGE_SCN_CNT_UNINITIALIZED_DATA | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE);

    // constant section
    /////////////////////////////////////////////
    concat.addString(".rdata", 8); // .Name
    concat.addU32(0);              // .VirtualSize
    concat.addU32(0);              // .VirtualAddress
    concat.addU32(0);              // .SizeOfRawData
    concat.addU32(0);              // .PointerToRawData
    concat.addU32(0);              // .PointerToRelocations
    concat.addU32(0);              // .PointerToLinenumbers
    concat.addU16(0);              // .NumberOfRelocations
    concat.addU16(0);              // .NumberOfLinenumbers
    concat.addU32(IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ);

    // mutable section
    /////////////////////////////////////////////
    concat.addString(".data", 8); // .Name
    concat.addU32(0);             // .VirtualSize
    concat.addU32(0);             // .VirtualAddress
    concat.addU32(0);             // .SizeOfRawData
    concat.addU32(0);             // .PointerToRawData
    concat.addU32(0);             // .PointerToRelocations
    concat.addU32(0);             // .PointerToLinenumbers
    concat.addU16(0);             // .NumberOfRelocations
    concat.addU16(0);             // .NumberOfLinenumbers
    concat.addU32(IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ);

    // Code section
    /////////////////////////////////////////////
    concat.addString(".text", 8); // .Name
    concat.addU32(0);             // .VirtualSize
    concat.addU32(0);             // .VirtualAddress

    addPatch(pp, PatchType::TextSectionSize, concat.addU32Addr(0));   // .SizeOfRawData
    addPatch(pp, PatchType::TextSectionOffset, concat.addU32Addr(0)); // .PointerToRawData

    concat.addU32(0); // .PointerToRelocations
    concat.addU32(0); // .PointerToLinenumbers
    concat.addU16(0); // .NumberOfRelocations
    concat.addU16(0); // .NumberOfLinenumbers
    concat.addU32(IMAGE_SCN_CNT_CODE | IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_MEM_READ);

    return true;
}

bool BackendX64::emitSymbolTable(const BuildParameters& buildParameters)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = perThread[ct][precompileIndex];
    auto& concat          = pp.concat;

    applyPatch(pp, PatchType::SymbolTableOffset, concat.totalCount);
    applyPatch(pp, PatchType::SymbolTableCount, 1);

    for (auto& symbol : pp.allSymbols)
    {
        SWAG_ASSERT(symbol.name.length() < 8);
        concat.addString(symbol.name.c_str(), 8); // .Name
        concat.addU32(symbol.value);              // .Value
        switch (symbol.kind)
        {
        case CoffSymbolKind::Function:
            concat.addU16(4);                             // .SectionNumber
            concat.addU16(IMAGE_SYM_DTYPE_FUNCTION << 8); // .Type
            concat.addU8(IMAGE_SYM_CLASS_EXTERNAL);       // .StorageClass
            break;
        default:
            SWAG_ASSERT(false);
            break;
        }
    }

    concat.addU8(0); // .NumberOfAuxSymbols

    return true;
}

bool BackendX64::emitStringTable(const BuildParameters& buildParameters)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = perThread[ct][precompileIndex];
    auto& concat          = pp.concat;

    concat.addU32(4); // .Size of table in bytes + 4
    return true;
}

bool BackendX64::generateObjFile(const BuildParameters& buildParameters)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = perThread[ct][precompileIndex];

    auto targetPath = Backend::getCacheFolder(buildParameters);
    auto path       = targetPath + "/" + pp.filename;
    auto filename   = path;

    ofstream destFile(filename, ios::binary);

    auto bucket = pp.concat.firstBucket;
    while (bucket)
    {
        destFile.write((const char*) bucket->datas, bucket->count);
        bucket = bucket->nextBucket;
    }

    destFile.flush();
    destFile.close();

    return true;
}

bool BackendX64::compile(const BuildParameters& buildParameters)
{
    // Do we need to generate the file ?
    if (!mustCompile)
        return true;

    vector<string> files;
    files.reserve(numPreCompileBuffers);
    for (auto i = 0; i < numPreCompileBuffers; i++)
        files.push_back(perThread[buildParameters.compileType][i].filename);
    return OS::link(buildParameters, module, files);
}
