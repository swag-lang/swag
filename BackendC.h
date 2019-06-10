#pragma once
#include "Concat.h"
struct Module;

struct BackendC
{
    void generate(Module* module);

    Concat outputH;
    Concat outputC;
};
