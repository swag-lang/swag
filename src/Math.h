#pragma once
#include "AstNode.h"
#include "ByteCodeInstruction.h"
#include "Module.h"
#include "SourceFile.h"

inline bool isPowerOfTwo(size_t v)
{
    if (!v)
        return false;
    return (v & (v - 1)) == 0;
}

inline bool overflowIsEnabled(ByteCodeInstruction* ip, AstNode* node)
{
    if (ip && ip->flags & BCI_CAN_OVERFLOW)
        return true;
    if (node && node->attributeFlags & ATTRIBUTE_CAN_OVERFLOW_ON)
        return true;
    if (!node->sourceFile->module->mustEmitSafetyOverflow(node))
        return true;
    return false;
}

inline bool addWillOverflow(ByteCodeInstruction* ip, AstNode* node, int8_t x, int8_t y)
{
    if (!overflowIsEnabled(ip, node))
    {
        const int32_t result = (int32_t) x + (int32_t) y;
        if (result < INT8_MIN || result > INT8_MAX)
            return true;
    }

    return false;
}

inline bool addWillOverflow(ByteCodeInstruction* ip, AstNode* node, int16_t x, int16_t y)
{
    if (!overflowIsEnabled(ip, node))
    {
        const int32_t result = (int32_t) x + (int32_t) y;
        if (result < INT16_MIN || result > INT16_MAX)
            return true;
    }

    return false;
}

inline bool addWillOverflow(ByteCodeInstruction* ip, AstNode* node, int32_t x, int32_t y)
{
    if (!overflowIsEnabled(ip, node))
    {
        const int64_t result = (int64_t) x + (int64_t) y;
        if (result < INT32_MIN || result > INT32_MAX)
            return true;
    }

    return false;
}

inline bool addWillOverflow(ByteCodeInstruction* ip, AstNode* node, int64_t x, int64_t y)
{
    if (!overflowIsEnabled(ip, node))
    {
        if (y < 0 && x < INT64_MIN - y)
            return true;
        if (y > 0 && x > INT64_MAX - y)
            return true;
    }

    return false;
}

inline bool addWillOverflow(ByteCodeInstruction* ip, AstNode* node, uint8_t x, uint8_t y)
{
    if (!overflowIsEnabled(ip, node))
    {
        const uint32_t result = (uint32_t) x + (uint32_t) y;
        if (result > UINT8_MAX)
            return true;
    }

    return false;
}

inline bool addWillOverflow(ByteCodeInstruction* ip, AstNode* node, uint16_t x, uint16_t y)
{
    if (!overflowIsEnabled(ip, node))
    {
        const uint32_t result = (uint32_t) x + (uint32_t) y;
        if (result > UINT16_MAX)
            return true;
    }

    return false;
}

inline bool addWillOverflow(ByteCodeInstruction* ip, AstNode* node, uint32_t x, uint32_t y)
{
    if (!overflowIsEnabled(ip, node))
    {
        const uint64_t result = (uint64_t) x + (uint64_t) y;
        if (result > UINT32_MAX)
            return true;
    }

    return false;
}

inline bool addWillOverflow(ByteCodeInstruction* ip, AstNode* node, uint64_t x, uint64_t y)
{
    if (!overflowIsEnabled(ip, node))
    {
        if (y > UINT64_MAX - x)
            return true;
    }

    return false;
}

inline bool subWillOverflow(ByteCodeInstruction* ip, AstNode* node, int8_t x, int8_t y)
{
    if (!overflowIsEnabled(ip, node))
    {
        const int32_t result = (int32_t) x - (int32_t) y;
        if (result < INT8_MIN || result > INT8_MAX)
            return true;
    }

    return false;
}

inline bool subWillOverflow(ByteCodeInstruction* ip, AstNode* node, int16_t x, int16_t y)
{
    if (!overflowIsEnabled(ip, node))
    {
        const int32_t result = (int32_t) x - (int32_t) y;
        if (result < INT16_MIN || result > INT16_MAX)
            return true;
    }

    return false;
}

inline bool subWillOverflow(ByteCodeInstruction* ip, AstNode* node, int32_t x, int32_t y)
{
    if (!overflowIsEnabled(ip, node))
    {
        const int64_t result = (int64_t) x - (int64_t) y;
        if (result < INT32_MIN || result > INT32_MAX)
            return true;
    }

    return false;
}

inline bool subWillOverflow(ByteCodeInstruction* ip, AstNode* node, int64_t x, int64_t y)
{
    if (!overflowIsEnabled(ip, node))
    {
        if (-y < 0 && x < INT64_MIN + y)
            return true;
        if (-y > 0 && x > INT64_MAX + y)
            return true;
    }
    return false;
}

inline bool subWillOverflow(ByteCodeInstruction* ip, AstNode* node, uint8_t x, uint8_t y)
{
    const uint32_t result = (uint32_t) x - (uint32_t) y;
    if (!overflowIsEnabled(ip, node))
    {
        if (result > UINT8_MAX)
            return true;
    }

    return false;
}

inline bool subWillOverflow(ByteCodeInstruction* ip, AstNode* node, uint16_t x, uint16_t y)
{
    const uint32_t result = (uint32_t) x - (uint32_t) y;
    if (!overflowIsEnabled(ip, node))
    {
        if (result > UINT16_MAX)
            return true;
    }

    return false;
}

inline bool subWillOverflow(ByteCodeInstruction* ip, AstNode* node, uint32_t x, uint32_t y)
{
    const uint64_t result = (uint64_t) x - (uint64_t) y;
    if (!overflowIsEnabled(ip, node))
    {
        if (result > UINT32_MAX)
            return true;
    }

    return false;
}

inline bool subWillOverflow(ByteCodeInstruction* ip, AstNode* node, uint64_t x, uint64_t y)
{
    if (!overflowIsEnabled(ip, node))
    {
        if (y > x)
            return true;
    }

    return false;
}

inline bool mulWillOverflow(ByteCodeInstruction* ip, AstNode* node, int8_t x, int8_t y)
{
    if (!overflowIsEnabled(ip, node))
    {
        const int32_t result = (int32_t) x * (int32_t) y;
        if (result < INT8_MIN || result > INT8_MAX)
            return true;
    }

    return false;
}

inline bool mulWillOverflow(ByteCodeInstruction* ip, AstNode* node, int16_t x, int16_t y)
{
    if (!overflowIsEnabled(ip, node))
    {
        const int32_t result = (int32_t) x * (int32_t) y;
        if (result < INT16_MIN || result > INT16_MAX)
            return true;
    }

    return false;
}

inline bool mulWillOverflow(ByteCodeInstruction* ip, AstNode* node, int32_t x, int32_t y)
{
    if (!overflowIsEnabled(ip, node))
    {
        const int64_t result = (int64_t) x * (int64_t) y;
        if (result < INT32_MIN || result > INT32_MAX)
            return true;
    }

    return false;
}

inline bool mulWillOverflow(ByteCodeInstruction* ip, AstNode* node, int64_t x, int64_t y)
{
    if (!overflowIsEnabled(ip, node))
    {
        if ((x > 0 && y > 0 && x > INT64_MAX / y) ||
            (x < 0 && y > 0 && x < INT64_MIN / y) ||
            (x > 0 && y < 0 && y < INT64_MIN / x) ||
            (x < 0 && y < 0 && (x <= INT64_MIN || y <= INT64_MIN || -x > INT64_MAX / -y)))
            return true;
    }

    return false;
}

inline bool mulWillOverflow(ByteCodeInstruction* ip, AstNode* node, uint8_t x, uint8_t y)
{
    if (!overflowIsEnabled(ip, node))
    {
        const uint32_t result = (uint32_t) x * (uint32_t) y;
        if (result > UINT8_MAX)
            return true;
    }

    return false;
}

inline bool mulWillOverflow(ByteCodeInstruction* ip, AstNode* node, uint16_t x, uint16_t y)
{
    if (!overflowIsEnabled(ip, node))
    {
        const uint32_t result = (uint32_t) x * (uint32_t) y;
        if (result > UINT16_MAX)
            return true;
    }

    return false;
}

inline bool mulWillOverflow(ByteCodeInstruction* ip, AstNode* node, uint32_t x, uint32_t y)
{
    if (!overflowIsEnabled(ip, node))
    {
        const uint64_t result = (uint64_t) x * (uint64_t) y;
        if (result > UINT32_MAX)
            return true;
    }

    return false;
}

inline bool mulWillOverflow(ByteCodeInstruction* ip, AstNode* node, uint64_t x, uint64_t y)
{
    if (!overflowIsEnabled(ip, node))
    {
        const auto res = x * y;
        if (res > 0 && (UINT64_MAX / y) < x)
            return true;
    }
    return false;
}
