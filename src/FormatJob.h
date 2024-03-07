#pragma once
#include "Job.h"

struct FormatJob final : Job
{
    JobResult execute() override;
    Path      fileName;
};
