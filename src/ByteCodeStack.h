#pragma once
#include "ByteCodeInstruction.h"
#include "ByteCodeRunContext.h"
#include "CommandLine.h"

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

    SWAG_FORCE_INLINE void push(ByteCodeRunContext* context)
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

    uint32_t maxLevel(ByteCodeRunContext* runContext);
    void     getSteps(VectorNative<ByteCodeStackStep>& copySteps, ByteCodeRunContext* runContext);
    Utf8     getStepName(AstNode* node, ByteCodeInstruction* ip);
    Utf8     getLogStep(int level, bool current, ByteCodeStackStep& step);
    Utf8     log(ByteCodeRunContext* runContext);

    VectorNative<ByteCodeStackStep> steps;
};

extern thread_local ByteCodeStack  g_ByteCodeStackTraceVal;
extern thread_local ByteCodeStack* g_ByteCodeStackTrace;
