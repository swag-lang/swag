#pragma once
#include "Threading/Job.h"

struct ByteCode;

struct ByteCodeSanityContext : JobContext
{
    ByteCode* bc = nullptr;
};

namespace ByteCodeSanity
{
    bool process(ByteCodeSanityContext* cxt);
};
