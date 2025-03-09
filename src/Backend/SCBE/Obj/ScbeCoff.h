#pragma once

struct BuildParameters;
struct Concat;
struct CpuRelocationTable;
struct ScbeCpu;
enum class CpuReg : uint8_t;

struct ScbeCoff
{
    static bool emitHeader(const BuildParameters& buildParameters, ScbeCpu& pp);
    static void emitUnwind(Concat& concat, uint32_t& offset, uint32_t sizeProlog, const VectorNative<uint16_t>& unwind);
    static bool emitRelocationTable(Concat& concat, const CpuRelocationTable& coffTable, uint32_t* sectionFlags, uint16_t* count);
    static bool emitPostFunc(const BuildParameters& buildParameters, ScbeCpu& pp);
    static void computeUnwind(const VectorNative<CpuReg>& unwindRegs, const VectorNative<uint32_t>& unwindOffsetRegs, uint32_t sizeStack, uint32_t offsetSubRSP, VectorNative<uint16_t>& unwind);
    static bool saveFileBuffer(FILE* f, const BuildParameters& buildParameters, ScbeCpu& pp);
};
