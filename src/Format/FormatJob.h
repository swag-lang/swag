#pragma once
#include "Threading/Job.h"

struct FormatJob final : Job
{
    JobResult execute() override;
    Path      fileName;
};
