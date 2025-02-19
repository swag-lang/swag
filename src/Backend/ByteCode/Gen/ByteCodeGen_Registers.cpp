#include "pch.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/ByteCode/Gen/ByteCodeGen.h"
#include "Backend/ByteCode/Gen/ByteCodeGenContext.h"
#include "Semantic/Symbol/Symbol.h"

void ByteCodeGen::reserveRegisterRC(const ByteCodeGenContext* context, RegisterList& rc, uint32_t num)
{
    rc.clear();
    rc.cannotFree = false;
    if (num == 0)
        return;
    if (num == 1)
        rc += reserveRegisterRC(context);
    else
    {
        SWAG_ASSERT(num == 2);
        reserveLinearRegisterRC2(context, rc);
    }
}

void ByteCodeGen::sortRegistersRC(const ByteCodeGenContext* context)
{
    if (!context->bc->isDirtyRegistersRC)
        return;
    context->bc->isDirtyRegistersRC = false;
    if (context->bc->availableRegistersRC.size() <= 1)
        return;
    std::ranges::sort(context->bc->availableRegistersRC, [](const uint32_t a, const uint32_t b) { return a > b; });
}

void ByteCodeGen::freeRegisterRC(const ByteCodeGenContext* context, uint32_t rc)
{
#ifdef SWAG_DEV_MODE
    for (const auto& r : context->bc->availableRegistersRC)
        SWAG_ASSERT(r != rc);
#endif

    context->bc->availableRegistersRC.push_back(rc);
    context->bc->isDirtyRegistersRC = true;
}

uint32_t ByteCodeGen::reserveRegisterRC(const ByteCodeGenContext* context, SymbolOverload* overload)
{
    uint32_t result     = UINT32_MAX;
    bool     hasRegInit = false;

    if (!context->bc->availableRegistersRC.empty())
    {
        sortRegistersRC(context);

        if (overload)
        {
            SharedLock lk(overload->symbol->mutex);
            if (overload->hasFlag(OVERLOAD_REG_HINT))
            {
                hasRegInit    = true;
                const auto it = context->bc->availableRegistersRC.find(overload->symRegisters[0]);
                if (it != -1)
                {
                    context->bc->availableRegistersRC.erase_unordered(it);
                    return overload->symRegisters[0];
                }
            }

            result = context->bc->availableRegistersRC.front();
            context->bc->availableRegistersRC.erase_unordered(0);
        }
        else
        {
            result = context->bc->availableRegistersRC.back();
            context->bc->availableRegistersRC.pop_back();
        }
    }
    else
    {
        result = context->bc->maxReservedRegisterRC++;
    }

    if (overload && !hasRegInit)
        overload->setRegisters(result, OVERLOAD_REG_HINT);
    return result;
}

void ByteCodeGen::reserveLinearRegisterRC2(const ByteCodeGenContext* context, RegisterList& rc)
{
    freeRegisterRC(context, rc);

    const auto size = context->bc->availableRegistersRC.size();
    if (size > 1)
    {
        sortRegistersRC(context);
        for (uint32_t i = 0; i < size - 1; i++)
        {
            if (context->bc->availableRegistersRC[i] == context->bc->availableRegistersRC[i + 1] + 1)
            {
                rc += context->bc->availableRegistersRC[i + 1];
                rc += context->bc->availableRegistersRC[i];
                context->bc->availableRegistersRC.erase(i, 2);
                return;
            }
        }
    }

    rc += context->bc->maxReservedRegisterRC++;
    rc += context->bc->maxReservedRegisterRC++;
}

void ByteCodeGen::transformResultToLinear2(const ByteCodeGenContext* context, RegisterList& resultRegisterRC)
{
    bool onlyOne = false;
    if (resultRegisterRC.size() == 1)
    {
        SWAG_ASSERT(!resultRegisterRC.cannotFree);
        onlyOne = true;

        sortRegistersRC(context);
        const auto it = context->bc->availableRegistersRC.find(resultRegisterRC[0] + 1);
        if (it != -1)
        {
            context->bc->availableRegistersRC.erase_unordered(it);
            resultRegisterRC += resultRegisterRC[0] + 1;
            return;
        }

        resultRegisterRC += reserveRegisterRC(context);
    }

    if (resultRegisterRC[1] != resultRegisterRC[0] + 1)
    {
        RegisterList r0;
        reserveLinearRegisterRC2(context, r0);
        EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, r0[0], resultRegisterRC[0]);
        if (!onlyOne)
            EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, r0[1], resultRegisterRC[1]);
        freeRegisterRC(context, resultRegisterRC);
        resultRegisterRC = r0;
    }
}

void ByteCodeGen::truncRegisterRC(const ByteCodeGenContext* context, RegisterList& rc, uint32_t count)
{
    if (rc.size() == count)
        return;

    SWAG_ASSERT(!rc.cannotFree);

    RegisterList rs;
    for (uint32_t i = 0; i < count; i++)
        rs += rc[i];

    if (!rc.cannotFree)
    {
        for (uint32_t i = count; i < rc.size(); i++)
            freeRegisterRC(context, rc[i]);
    }

    rs.cannotFree = rc.cannotFree;
    rc            = rs;
}

void ByteCodeGen::freeRegisterRC(const ByteCodeGenContext* context, RegisterList& rc)
{
    if (rc.cannotFree)
        return;
    for (uint32_t i = 0; i < rc.size(); i++)
        freeRegisterRC(context, rc[i]);
    rc.clear();
    rc.cannotFree = false;
}

void ByteCodeGen::freeRegisterRC(const ByteCodeGenContext* context, AstNode* node)
{
    if (!node)
        return;
    freeRegisterRC(context, node->resultRegisterRc);
    if (node->hasExtMisc())
        freeRegisterRC(context, node->extMisc()->additionalRegisterRC);
}

void ByteCodeGen::ensureCanBeChangedRC(const ByteCodeGenContext* context, RegisterList& r0)
{
    if (r0.cannotFree)
    {
        RegisterList re;
        reserveRegisterRC(context, re, r0.size());
        for (uint32_t i = 0; i < r0.size(); i++)
            EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, re[i], r0[i]);
        freeRegisterRC(context, r0);
        r0 = re;
    }
}
