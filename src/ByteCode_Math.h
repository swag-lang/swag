#pragma once
#include "AstNode.h"
#include "ByteCodeInstruction.h"
#include "Math.h"
#include "Module.h"
#include "SourceFile.h"

inline bool overflowIsEnabled(const ByteCodeInstruction* ip, const AstNode* node)
{
    if (ip && ip->flags & BCI_CAN_OVERFLOW)
        return true;
    if (node && node->hasAttribute(ATTRIBUTE_CAN_OVERFLOW_ON))
        return true;
    if (!node->sourceFile->module->mustEmitSafetyOverflow(node))
        return true;
    return false;
}

inline bool addWillOverflow(const ByteCodeInstruction* ip, const AstNode* node, int8_t x, int8_t y)
{
    if (!overflowIsEnabled(ip, node))
        return Math::addWillOverflow(x, y);
    return false;
}

inline bool addWillOverflow(const ByteCodeInstruction* ip, const AstNode* node, int16_t x, int16_t y)
{
    if (!overflowIsEnabled(ip, node))
        return Math::addWillOverflow(x, y);
    return false;
}

inline bool addWillOverflow(const ByteCodeInstruction* ip, const AstNode* node, int32_t x, int32_t y)
{
    if (!overflowIsEnabled(ip, node))
        return Math::addWillOverflow(x, y);
    return false;
}

inline bool addWillOverflow(const ByteCodeInstruction* ip, const AstNode* node, int64_t x, int64_t y)
{
    if (!overflowIsEnabled(ip, node))
        return Math::addWillOverflow(x, y);
    return false;
}

inline bool addWillOverflow(const ByteCodeInstruction* ip, const AstNode* node, uint8_t x, uint8_t y)
{
    if (!overflowIsEnabled(ip, node))
        return Math::addWillOverflow(x, y);
    return false;
}

inline bool addWillOverflow(const ByteCodeInstruction* ip, const AstNode* node, uint16_t x, uint16_t y)
{
    if (!overflowIsEnabled(ip, node))
        return Math::addWillOverflow(x, y);
    return false;
}

inline bool addWillOverflow(const ByteCodeInstruction* ip, const AstNode* node, uint32_t x, uint32_t y)
{
    if (!overflowIsEnabled(ip, node))
        return Math::addWillOverflow(x, y);
    return false;
}

inline bool addWillOverflow(const ByteCodeInstruction* ip, const AstNode* node, uint64_t x, uint64_t y)
{
    if (!overflowIsEnabled(ip, node))
        return Math::addWillOverflow(x, y);
    return false;
}

inline bool subWillOverflow(const ByteCodeInstruction* ip, const AstNode* node, int8_t x, int8_t y)
{
    if (!overflowIsEnabled(ip, node))
        return Math::subWillOverflow(x, y);
    return false;
}

inline bool subWillOverflow(const ByteCodeInstruction* ip, const AstNode* node, int16_t x, int16_t y)
{
    if (!overflowIsEnabled(ip, node))
        return Math::subWillOverflow(x, y);
    return false;
}

inline bool subWillOverflow(const ByteCodeInstruction* ip, const AstNode* node, int32_t x, int32_t y)
{
    if (!overflowIsEnabled(ip, node))
        return Math::subWillOverflow(x, y);
    return false;
}

inline bool subWillOverflow(const ByteCodeInstruction* ip, const AstNode* node, int64_t x, int64_t y)
{
    if (!overflowIsEnabled(ip, node))
        return Math::subWillOverflow(x, y);
    return false;
}

inline bool subWillOverflow(const ByteCodeInstruction* ip, const AstNode* node, uint8_t x, uint8_t y)
{
    if (!overflowIsEnabled(ip, node))
        return Math::subWillOverflow(x, y);
    return false;
}

inline bool subWillOverflow(const ByteCodeInstruction* ip, const AstNode* node, uint16_t x, uint16_t y)
{
    if (!overflowIsEnabled(ip, node))
        return Math::subWillOverflow(x, y);
    return false;
}

inline bool subWillOverflow(const ByteCodeInstruction* ip, const AstNode* node, uint32_t x, uint32_t y)
{
    if (!overflowIsEnabled(ip, node))
        return Math::subWillOverflow(x, y);
    return false;
}

inline bool subWillOverflow(const ByteCodeInstruction* ip, const AstNode* node, uint64_t x, uint64_t y)
{
    if (!overflowIsEnabled(ip, node))
        return Math::subWillOverflow(x, y);
    return false;
}

inline bool mulWillOverflow(const ByteCodeInstruction* ip, const AstNode* node, int8_t x, int8_t y)
{
    if (!overflowIsEnabled(ip, node))
        return Math::mulWillOverflow(x, y);
    return false;
}

inline bool mulWillOverflow(const ByteCodeInstruction* ip, const AstNode* node, int16_t x, int16_t y)
{
    if (!overflowIsEnabled(ip, node))
        return Math::mulWillOverflow(x, y);
    return false;
}

inline bool mulWillOverflow(const ByteCodeInstruction* ip, const AstNode* node, int32_t x, int32_t y)
{
    if (!overflowIsEnabled(ip, node))
        return Math::mulWillOverflow(x, y);
    return false;
}

inline bool mulWillOverflow(const ByteCodeInstruction* ip, const AstNode* node, int64_t x, int64_t y)
{
    if (!overflowIsEnabled(ip, node))
        return Math::mulWillOverflow(x, y);
    return false;
}

inline bool mulWillOverflow(const ByteCodeInstruction* ip, const AstNode* node, uint8_t x, uint8_t y)
{
    if (!overflowIsEnabled(ip, node))
        return Math::mulWillOverflow(x, y);
    return false;
}

inline bool mulWillOverflow(const ByteCodeInstruction* ip, const AstNode* node, uint16_t x, uint16_t y)
{
    if (!overflowIsEnabled(ip, node))
        return Math::mulWillOverflow(x, y);
    return false;
}

inline bool mulWillOverflow(const ByteCodeInstruction* ip, const AstNode* node, uint32_t x, uint32_t y)
{
    if (!overflowIsEnabled(ip, node))
        return Math::mulWillOverflow(x, y);
    return false;
}

inline bool mulWillOverflow(const ByteCodeInstruction* ip, const AstNode* node, uint64_t x, uint64_t y)
{
    if (!overflowIsEnabled(ip, node))
        return Math::mulWillOverflow(x, y);
    return false;
}
