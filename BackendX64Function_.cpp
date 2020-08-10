#include "pch.h"
#include "BackendX64.h"
#include "BackendX64Inst.h"
#include "BackendX64FunctionBodyJob.h"
#include "ByteCode.h"
#include "ByteCodeOp.h"
#include "Ast.h"
#include "TypeInfo.h"

BackendFunctionBodyJob* BackendX64::newFunctionJob()
{
    return g_Pool_backendX64FunctionBodyJob.alloc();
}

extern "C" void tt1(const char*, int)
{

}

extern char ___c[20];

void tt()
{
    tt1(___c + 5, 666);
}

bool BackendX64::emitFunctionBody(const BuildParameters& buildParameters, Module* moduleToGen, ByteCode* bc)
{
    tt();

    // Do not emit a text function if we are not compiling a test executable
    if (bc->node && (bc->node->attributeFlags & ATTRIBUTE_TEST_FUNC) && (buildParameters.compileType != BackendCompileType::Test))
        return true;

    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = perThread[ct][precompileIndex];
    auto& concat          = pp.concat;
    auto  typeFunc        = bc->callType();

    // Symbol
    getOrAddSymbol(pp, bc->callName(), CoffSymbolKind::Function, concat.totalCount - pp.textSectionOffset);

    // Reserve registers
    uint32_t sizeStack   = 0;
    uint32_t offsetStack = 0;
    uint32_t offsetRT    = 0;

    if (typeFunc->stackSize)
    {
        sizeStack += typeFunc->stackSize;
    }

    if (bc->maxCallResults)
    {
        offsetStack += bc->maxCallResults * sizeof(Register);
        sizeStack += bc->maxCallResults * sizeof(Register);
    }

    if (bc->maxReservedRegisterRC)
    {
        offsetStack += bc->maxReservedRegisterRC * sizeof(Register);
        offsetRT += bc->maxReservedRegisterRC * sizeof(Register);
        sizeStack += bc->maxReservedRegisterRC * sizeof(Register);
    }

    if (sizeStack)
    {
        // push rdi
        concat.addU8(0x57);

        // sub rsp, sizestack
        concat.addString("\x48\x81\xEC", 3);
        concat.addU32(sizeStack);

        // mov rdi, rsp
        concat.addString("\x48\x89\xE7", 3);
    }

    auto                   ip = bc->out;
    VectorNative<uint32_t> pushRAParams;
    for (uint32_t i = 0; i < bc->numInstructions; i++, ip++)
    {
        if (ip->node->flags & AST_NO_BACKEND)
        {
            SWAG_ASSERT(!(ip->flags & BCI_JUMP_DEST));
            continue;
        }

        switch (ip->op)
        {
        case ByteCodeOp::End:
            continue;
        }

        switch (ip->op)
        {
        case ByteCodeOp::MakeConstantSegPointer:
            //concat.addStringFormat("r[%u].pointer = (__u8_t*) (__cs + %u); ", ip->a.u32, ip->b.u32);

            // lea rax, [rip + __cs]
            concat.addString("\x48\x8D\x05", 3);
            CoffRelocation reloc;
            reloc.virtualAddress = concat.totalCount - pp.textSectionOffset;
            reloc.symbolIndex    = getSymbol(pp, "__cs")->index;
            reloc.type           = IMAGE_REL_AMD64_REL32;
            pp.relocTableTextSection.table.push_back(reloc);
            concat.addU32(0);
            
            // add rax, ip->b.u32
            concat.addString("\x48\x05", 2);
            concat.addU32(ip->b.u32);

            // mov qword ptr [rdi + ip->a.u32], rax
            concat.addString("\x48\x89\x87", 3);
            concat.addU32(ip->a.u32 * sizeof(Register));

            break;

        case ByteCodeOp::CopyVBtoRA32:
            //concat.addStringFormat("r[%u].u32 = 0x%x;", ip->a.u32, ip->b.u32);
            // mov dword ptr[rdi + ?] = ?
            concat.addString("\xC7\x87", 2);
            concat.addU32(ip->a.u32 * sizeof(Register));
            concat.addU32(ip->b.u32);
            break;

        case ByteCodeOp::IntrinsicPrintString:
            //swag_runtime_print_n(r[%u].pointer, r[%u].u32);", ip->a.u32, ip->b.u32);

            // mov edx, [rdi + reg]
            concat.addString("\x8B\x97", 2);
            concat.addU32(ip->b.u32 * sizeof(Register));

            // mov rcx, [rdi + reg]
            concat.addString("\x48\x8B\x8F", 3);
            concat.addU32(ip->a.u32 * sizeof(Register));

            emitCall(pp, "swag_runtime_print_n");
            break;

        case ByteCodeOp::Ret:
            if (sizeStack)
            {
                // add rsp, sizestack
                concat.addString("\x48\x81\xC4", 3);
                concat.addU32(sizeStack);

                // pop rdi
                concat.addU8(0x5F);
            }

            // ret
            concat.addU8(0xc3);
            break;
        }
    }

    return true;
}
