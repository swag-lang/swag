#pragma once
#include "FormatAst.h"
#include "Threading/Job.h"

struct FormatJob final : Job
{
    JobResult   execute() override;
    static bool getFormattedCode(FormatContext& context, const Path& fileName, Utf8& result);


    Path fileName;
};
