#include "pch.h"
#include "SourceFile.h"
#include "BackendC.h"
#include "BackendCCompilerVS.h"
#include "Global.h"
#include "Module.h"

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
	string tmpFolder = "f:/temp/";

    // Output C files
    destHFile  = tmpFolder + module->name + ".h";
    destCFile  = tmpFolder + module->name + ".c";
    outputFile = tmpFolder + module->name + ".exe";
    SWAG_CHECK(writeFile(destHFile.string().c_str(), outputH));
    SWAG_CHECK(writeFile(destCFile.string().c_str(), outputC));

    BackendCCompilerVS compiler(this);
    SWAG_CHECK(compiler.compile());

	return true;
}

bool BackendC::generate()
{
	outputC.addString(string("void main(){ return 0; }\n"));
    SWAG_CHECK(compile());
    return true;
}