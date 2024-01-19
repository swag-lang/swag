#pragma once
#include "Backend.h"
#include "BackendParameters.h"
#include "DataSegment.h"
#include "SCBE_CPU.h"

struct Module;
struct BuildParameters;
struct Job;
struct DataSegment;
struct ByteCode;
struct TypeInfo;
struct ByteCodeInstruction;
struct CPURelocationTable;

struct SCBE_Coff
{
    static bool emitHeader(const BuildParameters& buildParameters, SCBE_CPU& pp);
    static void emitUnwind(Concat& concat, uint32_t& offset, uint32_t sizeProlog, const VectorNative<uint16_t>& unwind);
    static bool emitXData(const BuildParameters& buildParameters, SCBE_CPU& pp);
    static bool emitPData(const BuildParameters& buildParameters, SCBE_CPU& pp);
    static bool emitDirectives(const BuildParameters& buildParameters, SCBE_CPU& pp);
    static bool emitSymbolTable(const BuildParameters& buildParameters, SCBE_CPU& pp);
    static bool emitStringTable(const BuildParameters& buildParameters, SCBE_CPU& pp);
    static bool emitRelocationTable(Concat& concat, CPURelocationTable& cofftable, uint32_t* sectionFlags, uint16_t* count);
    static bool emitPostFunc(const BuildParameters& buildParameters, SCBE_CPU& pp);

    static void computeUnwind(const VectorNative<CPURegister>& unwindRegs, const VectorNative<uint32_t>& unwindOffsetRegs, uint32_t sizeStack, uint32_t offsetSubRSP, VectorNative<uint16_t>& unwind);
    static bool saveFileBuffer(FILE* f, const BuildParameters& buildParameters, SCBE_CPU& pp);
};