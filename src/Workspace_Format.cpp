#include "pch.h"
#include "CommandLine.h"
#include "ErrorIds.h"
#include "FormatJob.h"
#include "LanguageSpec.h"
#include "Os.h"
#include "Report.h"
#include "TypeManager.h"
#include "Workspace.h"

void Workspace::formatCommand()
{
    g_LangSpec = Allocator::alloc<LanguageSpec>();
    g_LangSpec->setup();
    g_TypeMgr = Allocator::alloc<TypeManager>();
    g_TypeMgr->setup();

    if (!g_CommandLine.fileName.empty())
    {
        Path filePath = g_CommandLine.fileName;

        // Verify that the file exists
        std::error_code err;
        if (!std::filesystem::exists(filePath, err))
        {
            filePath             = std::filesystem::absolute(filePath);
            const auto filePath1 = std::filesystem::canonical(filePath, err);
            if (!err)
                filePath = filePath1;
            if (!std::filesystem::exists(filePath, err))
            {
                Report::error(formErr(Fat0030, filePath.c_str()));
                OS::exit(-1);
            }
        }

        FormatJob job;
        job.fileName = filePath;
        job.execute();
    }
    else
    {
        Report::error(formErr(Fat0037));
        OS::exit(-1);
    }
}
