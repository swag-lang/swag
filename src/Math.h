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

inline bool addOverflow(AstNode* node, int8_t x, int8_t y)
{
    if (node->sourceFile->module->mustEmitSafetyOF(node))
    {
        int32_t result = (int32_t) x + (int32_t) y;
        if (result < INT8_MIN || result > INT8_MAX)
            return true;
    }

    return false;
}

inline bool addOverflow(AstNode* node, int16_t x, int16_t y)
{
    if (node->sourceFile->module->mustEmitSafetyOF(node))
    {
        int32_t result = (int32_t) x + (int32_t) y;
        if (result < INT16_MIN || result > INT16_MAX)
            return true;
    }

    return false;
}

inline bool addOverflow(AstNode* node, int32_t x, int32_t y)
{
    if (node->sourceFile->module->mustEmitSafetyOF(node))
    {
        int64_t result = (int64_t) x + (int64_t) y;
        if (result < INT32_MIN || result > INT32_MAX)
            return true;
    }

    return false;
}

inline bool addOverflow(AstNode* node, int64_t x, int64_t y)
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

inline bool addOverflow(AstNode* node, uint8_t x, uint8_t y)
{
    if (node->sourceFile->module->mustEmitSafetyOF(node))
    {
        uint32_t result = (uint32_t) x + (uint32_t) y;
        if (result > UINT8_MAX)
            return true;
    }

    return false;
}

inline bool addOverflow(AstNode* node, uint16_t x, uint16_t y)
{
    if (node->sourceFile->module->mustEmitSafetyOF(node))
    {
        uint32_t result = (uint32_t) x + (uint32_t) y;
        if (result > UINT16_MAX)
            return true;
    }

    return false;
}

inline bool addOverflow(AstNode* node, uint32_t x, uint32_t y)
{
    if (node->sourceFile->module->mustEmitSafetyOF(node))
    {
        uint64_t result = (uint64_t) x + (uint64_t) y;
        if (result > UINT32_MAX)
            return true;
    }

    return false;
}

inline bool addOverflow(AstNode* node, uint64_t x, uint64_t y)
{
    if (node->sourceFile->module->mustEmitSafetyOF(node))
    {
        if (y > UINT64_MAX - x)
            return true;
    }

    return false;
}

inline bool subOverflow(AstNode* node, int8_t x, int8_t y)
{
    if (node->sourceFile->module->mustEmitSafetyOF(node))
    {
        int32_t result = (int32_t) x - (int32_t) y;
        if (result < INT8_MIN || result > INT8_MAX)
            return true;
    }

    return false;
}

inline bool subOverflow(AstNode* node, int16_t x, int16_t y)
{
    if (node->sourceFile->module->mustEmitSafetyOF(node))
    {
        int32_t result = (int32_t) x - (int32_t) y;
        if (result < INT16_MIN || result > INT16_MAX)
            return true;
    }

    return false;
}

inline bool subOverflow(AstNode* node, int32_t x, int32_t y)
{
    if (node->sourceFile->module->mustEmitSafetyOF(node))
    {
        int64_t result = (int64_t) x - (int64_t) y;
        if (result < INT32_MIN || result > INT32_MAX)
            return true;
    }

    return false;
}

inline bool subOverflow(AstNode* node, int64_t x, int64_t y)
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

inline bool subOverflow(AstNode* node, uint8_t x, uint8_t y)
{
    uint32_t result = (uint32_t) x - (uint32_t) y;
    if (node->sourceFile->module->mustEmitSafetyOF(node))
    {
        if (result > UINT8_MAX)
            return true;
    }

    return false;
}

inline bool subOverflow(AstNode* node, uint16_t x, uint16_t y)
{
    uint32_t result = (uint32_t) x - (uint32_t) y;
    if (node->sourceFile->module->mustEmitSafetyOF(node))
    {
        if (result > UINT16_MAX)
            return true;
    }

    return false;
}

inline bool subOverflow(AstNode* node, uint32_t x, uint32_t y)
{
    uint64_t result = (uint64_t) x - (uint64_t) y;
    if (node->sourceFile->module->mustEmitSafetyOF(node))
    {
        if (result > UINT32_MAX)
            return true;
    }

    return false;
}

inline bool subOverflow(AstNode* node, uint64_t x, uint64_t y)
{
    if (node->sourceFile->module->mustEmitSafetyOF(node))
    {
        if (y > x)
            return true;
    }

    return false;
}

inline bool mulOverflow(AstNode* node, int8_t x, int8_t y)
{
    if (node->sourceFile->module->mustEmitSafetyOF(node))
    {
        int32_t result = (int32_t) x * (int32_t) y;
        if (result < INT8_MIN || result > INT8_MAX)
            return true;
    }

    return false;
}

inline bool mulOverflow(AstNode* node, int16_t x, int16_t y)
{
    if (node->sourceFile->module->mustEmitSafetyOF(node))
    {
        int32_t result = (int32_t) x * (int32_t) y;
        if (result < INT16_MIN || result > INT16_MAX)
            return true;
    }

    return false;
}

inline bool mulOverflow(AstNode* node, int32_t x, int32_t y)
{
    if (node->sourceFile->module->mustEmitSafetyOF(node))
    {
        int64_t result = (int64_t) x * (int64_t) y;
        if (result < INT32_MIN || result > INT32_MAX)
            return true;
    }

    return false;
}

inline bool mulOverflow(AstNode* node, int64_t x, int64_t y)
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

inline bool mulOverflow(AstNode* node, uint8_t x, uint8_t y)
{
    if (node->sourceFile->module->mustEmitSafetyOF(node))
    {
        uint32_t result = (uint32_t) x * (uint32_t) y;
        if (result > UINT8_MAX)
            return true;
    }

    return false;
}

inline bool mulOverflow(AstNode* node, uint16_t x, uint16_t y)
{
    if (node->sourceFile->module->mustEmitSafetyOF(node))
    {
        uint32_t result = (uint32_t) x * (uint32_t) y;
        if (result > UINT16_MAX)
            return true;
    }

    return false;
}

inline bool mulOverflow(AstNode* node, uint32_t x, uint32_t y)
{
    if (node->sourceFile->module->mustEmitSafetyOF(node))
    {
        uint64_t result = (uint64_t) x * (uint64_t) y;
        if (result > UINT32_MAX)
            return true;
    }

    return false;
}

inline bool mulOverflow(AstNode* node, uint64_t x, uint64_t y)
{
    if (node->sourceFile->module->mustEmitSafetyOF(node))
    {
        auto res = x * y;
        if (res > 0 && (UINT64_MAX / y) < x)
            return true;
    }
    return false;
}

inline void executeShiftLeft(Register* rdest, const Register& rleft, const Register& rright, uint32_t numBits, bool isSmall)
{
    auto shift = rright.u32;
    if (isSmall)
        shift &= numBits - 1;

    if (shift >= numBits)
        rdest->u64 = 0;
    else
        rdest->u64 = rleft.u64 << shift;
}
inline void executeShiftRight(Register* rdest, const Register& rleft, const Register& rright, uint32_t numBits, bool isSigned, bool isSmall)
{
    auto shift = rright.u32;
    if (isSmall)
        shift &= numBits - 1;

    // Overflow, too many bits to shift
    if (shift >= numBits)
    {
        if (isSigned)
        {
            switch (numBits)
            {
            case 8:
                rdest->s64 = rleft.s8 < 0 ? -1 : 0;
                break;
            case 16:
                rdest->s64 = rleft.s16 < 0 ? -1 : 0;
                break;
            case 32:
                rdest->s64 = rleft.s32 < 0 ? -1 : 0;
                break;
            case 64:
                rdest->s64 = rleft.s64 < 0 ? -1 : 0;
                break;
            }
        }
        else
        {
            rdest->u64 = 0;
        }
    }
    else if (isSigned)
    {
        switch (numBits)
        {
        case 8:
            rdest->s64 = rleft.s8 >> shift;
            break;
        case 16:
            rdest->s64 = rleft.s16 >> shift;
            break;
        case 32:
            rdest->s64 = rleft.s32 >> shift;
            break;
        case 64:
            rdest->s64 = rleft.s64 >> shift;
            break;
        }
    }
    else
    {
        switch (numBits)
        {
        case 8:
            rdest->u64 = rleft.u8 >> shift;
            break;
        case 16:
            rdest->u64 = rleft.u16 >> shift;
            break;
        case 32:
            rdest->u64 = rleft.u32 >> shift;
            break;
        case 64:
            rdest->u64 = rleft.u64 >> shift;
            break;
        }
    }
}
