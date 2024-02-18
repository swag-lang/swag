#pragma once

struct BuildParameters;
struct Concat;
struct CPURelocationTable;
struct SCBE_CPU;
enum CPURegister : uint8_t;

static constexpr int UWOP_PUSH_NO_VOL = 0x00000000;
static constexpr int UWOP_ALLOC_LARGE = 0x00000001;
static constexpr int UWOP_ALLOC_SMALL = 0x00000002;

struct SCBE_Coff
{
    static bool emitHeader(const BuildParameters& buildParameters, SCBE_CPU& pp);
    static void emitUnwind(Concat& concat, uint32_t& offset, uint32_t sizeProlog, const VectorNative<uint16_t>& unwind);
    static bool emitRelocationTable(Concat& concat, const CPURelocationTable& coffTable, uint32_t* sectionFlags, uint16_t* count);
    static bool emitPostFunc(const BuildParameters& buildParameters, SCBE_CPU& pp);
    static void computeUnwind(const VectorNative<CPURegister>& unwindRegs, const VectorNative<uint32_t>& unwindOffsetRegs, uint32_t sizeStack, uint32_t offsetSubRSP, VectorNative<uint16_t>& unwind);
    static bool saveFileBuffer(FILE* f, const BuildParameters& buildParameters, SCBE_CPU& pp);
};
