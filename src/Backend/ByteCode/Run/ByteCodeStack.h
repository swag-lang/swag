#pragma once
#include "Backend/ByteCode/ByteCodeInstruction.h"
#include "Backend/ByteCode/Run/ByteCodeRunContext.h"

struct ByteCode;

struct ByteCodeStackStep
{
    ByteCode*            bc    = nullptr;
    ByteCodeInstruction* ip    = nullptr;
    uint8_t*             bp    = nullptr;
    uint8_t*             sp    = nullptr;
    uint8_t*             spAlt = nullptr;
    uint8_t*             stack = nullptr;
};

struct ByteCodeStack
{
    SWAG_FORCE_INLINE void push(const ByteCodeStackStep& step)
    {
        steps.push_back(step);
    }

    SWAG_FORCE_INLINE void push(const ByteCodeRunContext* context)
    {
        if (steps.count < steps.allocated)
        {
            const auto buf = steps.buffer + steps.count++;
            buf->bc        = context->bc;
            buf->ip        = context->ip - 1;
            buf->bp        = context->bp;
            buf->sp        = context->sp;
            buf->spAlt     = context->spAlt;
            buf->stack     = context->stack;
        }
        else
        {
            ByteCodeStackStep stackStep;
            stackStep.bc    = context->bc;
            stackStep.ip    = context->ip - 1;
            stackStep.bp    = context->bp;
            stackStep.sp    = context->sp;
            stackStep.spAlt = context->spAlt;
            stackStep.stack = context->stack;
            push(stackStep);
        }
    }

    SWAG_FORCE_INLINE void pop()
    {
        if (steps.count)
            steps.count--;
    }

    ByteCodeStackStep& last()
    {
        return steps.back();
    }

    void clear()
    {
        steps.clear();
    }

    uint32_t    maxLevel(const ByteCodeRunContext* runContext);
    void        getSteps(VectorNative<ByteCodeStackStep>& copySteps, const ByteCodeRunContext* runContext) const;
    static Utf8 getStepName(const AstNode* node, const ByteCodeInstruction* ip);
    static Utf8 getLogStep(uint32_t level, bool current, ByteCodeStackStep& step, bool sourceCode);
    Utf8        log(const ByteCodeRunContext* runContext, uint32_t maxSteps = 20, bool sourceCode = false) const;

    VectorNative<ByteCodeStackStep> steps;
};

extern thread_local ByteCodeStack  g_ByteCodeStackTraceVal;
extern thread_local ByteCodeStack* g_ByteCodeStackTrace;
