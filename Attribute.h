#pragma once
#include "Pool.h"
#include "Utf8.h"

static const uint32_t ATTRIBUTE_FUNC = 0x00000001;
static const uint32_t ATTRIBUTE_VAR  = 0x00000002;

struct Attribute : PoolElement
{
    Utf8     name;
    uint32_t flags;
};