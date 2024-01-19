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
    bool emitHeader(const BuildParameters& buildParameters, SCBE_CPU& pp);
    bool emitXData(const BuildParameters& buildParameters, SCBE_X64& pp);
    bool emitPData(const BuildParameters& buildParameters, SCBE_CPU& pp);
    bool emitDirectives(const BuildParameters& buildParameters, SCBE_CPU& pp);
    bool emitSymbolTable(const BuildParameters& buildParameters, SCBE_CPU& pp);
    bool emitStringTable(const BuildParameters& buildParameters, SCBE_CPU& pp);
    bool emitRelocationTable(SCBE_CPU& pp, CPURelocationTable& cofftable, uint32_t* sectionFlags, uint16_t* count);

    bool emitBuffer(FILE* f, const BuildParameters& buildParameters, SCBE_CPU& pp);

    SCBE* scbe = nullptr;
};