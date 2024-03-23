#pragma once
#include "FormatAst.h"
#include "Threading/Job.h"

struct FormatJob final : Job
{
    JobResult execute() override;

    static bool getFormattedCode(const FormatOptions& options, const Path& fileName, Utf8& result);
    static bool writeResult(const Path& fileName, const Utf8& content);

    FormatOptions options;
    Path          fileName;
};
