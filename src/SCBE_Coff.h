#pragma once

struct BuildParameters;
struct CPURelocationTable;
struct SCBE_CPU;

#define UWOP_PUSH_NONVOL 0
#define UWOP_ALLOC_LARGE 1
#define UWOP_ALLOC_SMALL 2

struct SCBE_Coff
{
    static bool emitHeader(const BuildParameters& buildParameters, SCBE_CPU& pp);
    static void emitUnwind(Concat& concat, uint32_t& offset, uint32_t sizeProlog, const VectorNative<uint16_t>& unwind);
    static bool emitRelocationTable(Concat& concat, CPURelocationTable& cofftable, uint32_t* sectionFlags, uint16_t* count);
    static bool emitPostFunc(const BuildParameters& buildParameters, SCBE_CPU& pp);

    static void computeUnwind(const VectorNative<CPURegister>& unwindRegs, const VectorNative<uint32_t>& unwindOffsetRegs, uint32_t sizeStack, uint32_t offsetSubRSP, VectorNative<uint16_t>& unwind);
    static bool saveFileBuffer(FILE* f, const BuildParameters& buildParameters, SCBE_CPU& pp);
};