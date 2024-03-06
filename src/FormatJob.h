#pragma once
#include "Job.h"

struct FormatJob final : Job
{
    JobResult execute() override;
    Utf8      fileName;
};
