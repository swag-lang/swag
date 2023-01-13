#pragma once
#include "AstNode.h"
#include "SourceFile.h"
#include "Module.h"

inline bool isPowerOfTwo(size_t v)
{
    if (!v)
        return false;
    return (v & (v - 1)) == 0;
}

inline bool addWillOverflow(AstNode* node, int8_t x, int8_t y)
{
    if (node->sourceFile->module->mustEmitSafetyOF(node))
    {
        int32_t result = (int32_t) x + (int32_t) y;
        if (result < INT8_MIN || result > INT8_MAX)
            return true;
    }

    return false;
}

inline bool addWillOverflow(AstNode* node, int16_t x, int16_t y)
{
    if (node->sourceFile->module->mustEmitSafetyOF(node))
    {
        int32_t result = (int32_t) x + (int32_t) y;
        if (result < INT16_MIN || result > INT16_MAX)
            return true;
    }

    return false;
}

inline bool addWillOverflow(AstNode* node, int32_t x, int32_t y)
{
    if (node->sourceFile->module->mustEmitSafetyOF(node))
    {
        int64_t result = (int64_t) x + (int64_t) y;
        if (result < INT32_MIN || result > INT32_MAX)
            return true;
    }

    return false;
}

inline bool addWillOverflow(AstNode* node, int64_t x, int64_t y)
{
    if (node->sourceFile->module->mustEmitSafetyOF(node))
    {
        if (y < 0 && x < INT64_MIN - y)
            return true;
        if (y > 0 && x > INT64_MAX - y)
            return true;
    }

    return false;
}

inline bool addWillOverflow(AstNode* node, uint8_t x, uint8_t y)
{
    if (node->sourceFile->module->mustEmitSafetyOF(node))
    {
        uint32_t result = (uint32_t) x + (uint32_t) y;
        if (result > UINT8_MAX)
            return true;
    }

    return false;
}

inline bool addWillOverflow(AstNode* node, uint16_t x, uint16_t y)
{
    if (node->sourceFile->module->mustEmitSafetyOF(node))
    {
        uint32_t result = (uint32_t) x + (uint32_t) y;
        if (result > UINT16_MAX)
            return true;
    }

    return false;
}

inline bool addWillOverflow(AstNode* node, uint32_t x, uint32_t y)
{
    if (node->sourceFile->module->mustEmitSafetyOF(node))
    {
        uint64_t result = (uint64_t) x + (uint64_t) y;
        if (result > UINT32_MAX)
            return true;
    }

    return false;
}

inline bool addWillOverflow(AstNode* node, uint64_t x, uint64_t y)
{
    if (node->sourceFile->module->mustEmitSafetyOF(node))
    {
        if (y > UINT64_MAX - x)
            return true;
    }

    return false;
}

inline bool subWillOverflow(AstNode* node, int8_t x, int8_t y)
{
    if (node->sourceFile->module->mustEmitSafetyOF(node))
    {
        int32_t result = (int32_t) x - (int32_t) y;
        if (result < INT8_MIN || result > INT8_MAX)
            return true;
    }

    return false;
}

inline bool subWillOverflow(AstNode* node, int16_t x, int16_t y)
{
    if (node->sourceFile->module->mustEmitSafetyOF(node))
    {
        int32_t result = (int32_t) x - (int32_t) y;
        if (result < INT16_MIN || result > INT16_MAX)
            return true;
    }

    return false;
}

inline bool subWillOverflow(AstNode* node, int32_t x, int32_t y)
{
    if (node->sourceFile->module->mustEmitSafetyOF(node))
    {
        int64_t result = (int64_t) x - (int64_t) y;
        if (result < INT32_MIN || result > INT32_MAX)
            return true;
    }

    return false;
}

inline bool subWillOverflow(AstNode* node, int64_t x, int64_t y)
{
    if (node->sourceFile->module->mustEmitSafetyOF(node))
    {
        if (-y < 0 && x < INT64_MIN + y)
            return true;
        if (-y > 0 && x > INT64_MAX + y)
            return true;
    }
    return false;
}

inline bool subWillOverflow(AstNode* node, uint8_t x, uint8_t y)
{
    uint32_t result = (uint32_t) x - (uint32_t) y;
    if (node->sourceFile->module->mustEmitSafetyOF(node))
    {
        if (result > UINT8_MAX)
            return true;
    }

    return false;
}

inline bool subWillOverflow(AstNode* node, uint16_t x, uint16_t y)
{
    uint32_t result = (uint32_t) x - (uint32_t) y;
    if (node->sourceFile->module->mustEmitSafetyOF(node))
    {
        if (result > UINT16_MAX)
            return true;
    }

    return false;
}

inline bool subWillOverflow(AstNode* node, uint32_t x, uint32_t y)
{
    uint64_t result = (uint64_t) x - (uint64_t) y;
    if (node->sourceFile->module->mustEmitSafetyOF(node))
    {
        if (result > UINT32_MAX)
            return true;
    }

    return false;
}

inline bool subWillOverflow(AstNode* node, uint64_t x, uint64_t y)
{
    if (node->sourceFile->module->mustEmitSafetyOF(node))
    {
        if (y > x)
            return true;
    }

    return false;
}

inline bool mulWillOverflow(AstNode* node, int8_t x, int8_t y)
{
    if (node->sourceFile->module->mustEmitSafetyOF(node))
    {
        int32_t result = (int32_t) x * (int32_t) y;
        if (result < INT8_MIN || result > INT8_MAX)
            return true;
    }

    return false;
}

inline bool mulWillOverflow(AstNode* node, int16_t x, int16_t y)
{
    if (node->sourceFile->module->mustEmitSafetyOF(node))
    {
        int32_t result = (int32_t) x * (int32_t) y;
        if (result < INT16_MIN || result > INT16_MAX)
            return true;
    }

    return false;
}

inline bool mulWillOverflow(AstNode* node, int32_t x, int32_t y)
{
    if (node->sourceFile->module->mustEmitSafetyOF(node))
    {
        int64_t result = (int64_t) x * (int64_t) y;
        if (result < INT32_MIN || result > INT32_MAX)
            return true;
    }

    return false;
}

inline bool mulWillOverflow(AstNode* node, int64_t x, int64_t y)
{
    if (node->sourceFile->module->mustEmitSafetyOF(node))
    {
        if ((x > 0 && y > 0 && x > INT64_MAX / y) ||
            (x < 0 && y > 0 && x < INT64_MIN / y) ||
            (x > 0 && y < 0 && y < INT64_MIN / x) ||
            (x < 0 && y < 0 && (x <= INT64_MIN || y <= INT64_MIN || -x > INT64_MAX / -y)))
            return true;
    }

    return false;
}

inline bool mulWillOverflow(AstNode* node, uint8_t x, uint8_t y)
{
    if (node->sourceFile->module->mustEmitSafetyOF(node))
    {
        uint32_t result = (uint32_t) x * (uint32_t) y;
        if (result > UINT8_MAX)
            return true;
    }

    return false;
}

inline bool mulWillOverflow(AstNode* node, uint16_t x, uint16_t y)
{
    if (node->sourceFile->module->mustEmitSafetyOF(node))
    {
        uint32_t result = (uint32_t) x * (uint32_t) y;
        if (result > UINT16_MAX)
            return true;
    }

    return false;
}

inline bool mulWillOverflow(AstNode* node, uint32_t x, uint32_t y)
{
    if (node->sourceFile->module->mustEmitSafetyOF(node))
    {
        uint64_t result = (uint64_t) x * (uint64_t) y;
        if (result > UINT32_MAX)
            return true;
    }

    return false;
}

inline bool mulWillOverflow(AstNode* node, uint64_t x, uint64_t y)
{
    if (node->sourceFile->module->mustEmitSafetyOF(node))
    {
        auto res = x * y;
        if (res > 0 && (UINT64_MAX / y) < x)
            return true;
    }
    return false;
}

inline bool shiftLeftHasOverflowed(AstNode* node, int8_t rdest, int8_t rleft, uint32_t rright, bool isSmall)
{
    if (node->sourceFile->module->mustEmitSafetyOF(node))
    {
        if (rright >= 8)
            return true;
        if ((rdest & 0x80) != (rleft & 0x80))
            return true;
    }

    return false;
}