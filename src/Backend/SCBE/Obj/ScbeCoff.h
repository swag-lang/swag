#pragma once

struct BuildParameters;
struct Concat;
struct CPURelocationTable;
struct ScbeCPU;
enum class CPUReg : uint8_t;

struct ScbeCoff
{
    static bool emitHeader(const BuildParameters& buildParameters, ScbeCPU& pp);
    static void emitUnwind(Concat& concat, uint32_t& offset, uint32_t sizeProlog, const VectorNative<uint16_t>& unwind);
    static bool emitRelocationTable(Concat& concat, const CPURelocationTable& coffTable, uint32_t* sectionFlags, uint16_t* count);
    static bool emitPostFunc(const BuildParameters& buildParameters, ScbeCPU& pp);
    static void computeUnwind(const VectorNative<CPUReg>& unwindRegs, const VectorNative<uint32_t>& unwindOffsetRegs, uint32_t sizeStack, uint32_t offsetSubRSP, VectorNative<uint16_t>& unwind);
    static bool saveFileBuffer(FILE* f, const BuildParameters& buildParameters, ScbeCPU& pp);
};
