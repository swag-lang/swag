#pragma once
#include "CommandLine.h"
#include "ByteCodeRunContext.h"
#include "ByteCodeInstruction.h"

struct ByteCode;

struct ByteCodeStackStep
{
    ByteCode*            bc = nullptr;
    ByteCodeInstruction* ip = nullptr;
    uint8_t*             bp = nullptr;
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
            auto buf = steps.buffer + steps.count++;
            buf->bc  = context->bc;
            buf->ip  = context->ip - 1;
            buf->bp  = context->bp;
        }
        else
        {
            ByteCodeStackStep stackStep;
            stackStep.bc = context->bc;
            stackStep.ip = context->ip - 1;
            stackStep.bp = context->bp;
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
        currentContext = nullptr;
    }

    void     reportError(const Utf8& msg);
    void     getSteps(VectorNative<ByteCodeStackStep>& copySteps);
    uint32_t maxLevel(ByteCodeRunContext* context);
    void     logStep(int level, bool current, ByteCodeStackStep& step);
    void     log();

    VectorNative<ByteCodeStackStep> steps;
    ByteCodeRunContext*             currentContext = nullptr;
};

extern thread_local ByteCodeStack g_ByteCodeStack;
