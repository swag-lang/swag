#include "pch.h"
#include "SourceFile.h"
#include "BackendC.h"
#include "BackendCCompilerVS.h"
#include "Global.h"
#include "Module.h"
#include "CommandLine.h"

bool BackendC::writeFile(const char* fileName, Concat& concat)
{
    FILE* file;
    fopen_s(&file, fileName, "wt");
    SWAG_VERIFY(file, module->error(format("can't open file '%s' for writing'", fileName)));

    auto bucket = concat.firstBucket;
    while (bucket)
    {
        int count = bucket->count;
        fwrite(bucket->datas, 1, count, file);
        bucket = bucket->nextBucket;
    }

    fclose(file);
    return true;
}

bool BackendC::compile()
{
    if (module->buildPass == BuildPass::Full)
    {
        BackendCCompilerVS compiler(this);
        SWAG_CHECK(compiler.compile());
        SWAG_CHECK(compiler.runTests());
    }

    return true;
}

bool BackendC::generate()
{
    bool ok = true;

    ok &= emitRuntime();
    ok &= emitDataSegment();
	ok &= emitFuncSignatures();
    ok &= emitFunctions();
    ok &= emitMain();

    string tmpFolder = "f:/temp/";
    destHFile        = tmpFolder + module->name + ".h";
    destCFile        = tmpFolder + module->name + ".c";
    outputFile       = tmpFolder + module->name + ".exe";
    SWAG_CHECK(writeFile(destHFile.string().c_str(), outputH));
    SWAG_CHECK(writeFile(destCFile.string().c_str(), outputC));

    SWAG_CHECK(ok);
    SWAG_CHECK(compile());
    return true;
}