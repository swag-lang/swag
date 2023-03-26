#pragma once
#include "Utf8.h"
#include "Backend.h"
#include "BackendParameters.h"
#include "DataSegment.h"
#include "CallConv.h"

struct Module;
struct BuildParameters;
struct Job;
struct DataSegment;
struct ByteCode;
struct TypeInfo;
struct ByteCodeInstruction;

#define regOffset(__r) __r * sizeof(Register)

#define MK_ALIGN16(__s) \
    if (__s % 16)       \
        __s += 16 - (__s % 16);

enum class X64PushParamType
{
    Reg,
    RegAdd,
    RegMul,
    Imm,
    Imm64,
    RelocV,
    RelocAddr,
    Addr,
    GlobalString,
};

struct X64PushParam
{
    X64PushParamType type = X64PushParamType::Reg;
    uint64_t         reg  = 0;
    uint64_t         val  = 0;
};

enum class X64Op : uint8_t
{
    ADD   = 0x01,
    OR    = 0x09,
    AND   = 0x21,
    SUB   = 0x29,
    XOR   = 0x31,
    IDIV  = 0xF7,
    MUL   = 0xC0,
    IMUL  = 0xC1,
    FADD  = 0x58,
    FSUB  = 0x5C,
    FMUL  = 0x59,
    FDIV  = 0x5E,
    XCHG  = 0x87,
    SHL   = 0xE0,
    SHR   = 0xE8,
    SHLEQ = 0x20,
    SHREQ = 0x28,
};

enum class CoffSymbolKind
{
    Function,
    Extern,
    Custom,
    GlobalString,
};

enum Disp
{
    DISP8  = 0b01,
    DISP32 = 0b10,
    REGREG = 0b11,
};

enum JumpType
{
    JNO,
    JNZ,
    JZ,
    JL,
    JLE,
    JB,
    JBE,
    JGE,
    JAE,
    JG,
    JA,
    JP,
    JNP,
    JUMP,
};

struct CoffSymbol
{
    Utf8           name;
    CoffSymbolKind kind;
    uint32_t       value;
    uint32_t       index;
    uint16_t       sectionIdx;
};

struct CoffRelocation
{
    uint32_t virtualAddress;
    uint32_t symbolIndex;
    uint16_t type;
};

struct CoffRelocationTable
{
    Vector<CoffRelocation> table;
};

struct LabelToSolve
{
    uint32_t ipDest;
    int32_t  currentOffset;
    uint8_t* patch;
};

using DbgTypeIndex = uint32_t;
struct DbgTypeRecordArgList
{
    Vector<DbgTypeIndex> args;
    uint32_t             count = 0;
};

struct DbgTypeRecordProcedure
{
    DbgTypeIndex returnType = 0;
    DbgTypeIndex argsType   = 0;
    uint16_t     numArgs    = 0;
};

struct DbgTypeRecordMFunction
{
    DbgTypeIndex returnType = 0;
    DbgTypeIndex structType = 0;
    DbgTypeIndex thisType   = 0;
    DbgTypeIndex argsType   = 0;
    uint16_t     numArgs    = 0;
};

struct DbgTypeRecordFuncId
{
    DbgTypeIndex type = 0;
};

struct DbgTypeRecordMFuncId
{
    DbgTypeIndex parentType = 0;
    DbgTypeIndex type       = 0;
};

struct DbgTypeField
{
    Utf8          name;
    ComputedValue value;
    TypeInfo*     valueType       = nullptr;
    DbgTypeIndex  type            = 0;
    uint16_t      accessSpecifier = 0;
    uint16_t      kind            = 0;
};

struct DbgTypeRecordFieldList
{
    Vector<DbgTypeField> fields;
};

struct DbgTypeRecordDerivedList
{
    Vector<DbgTypeIndex> derived;
};

struct DbgTypeRecordStructure
{
    DbgTypeIndex fieldList   = 0;
    DbgTypeIndex derivedList = 0;
    uint32_t     sizeOf      = 0;
    uint16_t     memberCount = 0;
    bool         forward     = false;
};

struct DbgTypeRecordEnum
{
    DbgTypeIndex fieldList      = 0;
    DbgTypeIndex underlyingType = 0;
    uint16_t     count          = 0;
};

struct DbgTypeRecordArray
{
    DbgTypeIndex elementType = 0;
    DbgTypeIndex indexType   = 0;
    uint32_t     sizeOf      = 0;
};

struct DbgTypeRecordPointer
{
    DbgTypeIndex pointeeType = 0;
    bool         asRef       = false;
};

struct DbgTypeRecord
{
    Utf8                     name;
    AstNode*                 node = nullptr;
    DbgTypeRecordArgList     LF_ArgList;
    DbgTypeRecordProcedure   LF_Procedure;
    DbgTypeRecordMFunction   LF_MFunction;
    DbgTypeRecordFuncId      LF_FuncId;
    DbgTypeRecordMFuncId     LF_MFuncId;
    DbgTypeRecordFieldList   LF_FieldList;
    DbgTypeRecordDerivedList LF_DerivedList;
    DbgTypeRecordStructure   LF_Structure;
    DbgTypeRecordArray       LF_Array;
    DbgTypeRecordPointer     LF_Pointer;
    DbgTypeRecordEnum        LF_Enum;
    DbgTypeIndex             index = 0;
    uint16_t                 kind  = 0;
};

struct DbgLine
{
    uint32_t line;
    uint32_t byteOffset;
};

struct DbgLines
{
    SourceFile*     sourceFile;
    Vector<DbgLine> dbgLines;
};

struct CoffFunction
{
    VectorNative<uint16_t> unwind;
    Vector<DbgLines>       dbgLines;
    AstNode*               node                    = nullptr;
    TypeInfoFuncAttr*      typeFunc                = nullptr;
    uint32_t               symbolIndex             = 0;
    uint32_t               startAddress            = 0;
    uint32_t               endAddress              = 0;
    uint32_t               xdataOffset             = 0;
    uint32_t               sizeProlog              = 0;
    uint32_t               offsetStack             = 0;
    uint32_t               offsetCallerStackParams = 0;
    uint32_t               offsetLocalStackParams  = 0;
    uint32_t               frameSize               = 0;
    uint32_t               numScratchRegs          = 0;
};

struct X64Gen
{
    Utf8   filename;
    Concat concat;
    Concat postConcat;

    VectorNative<const Utf8*>  stringTable;
    VectorNative<X64PushParam> pushParams;
    VectorNative<X64PushParam> pushParams2;
    VectorNative<X64PushParam> pushParams3;
    VectorNative<TypeInfo*>    pushParamsTypes;
    CoffRelocationTable        relocTableTextSection;
    CoffRelocationTable        relocTableCSSection;
    CoffRelocationTable        relocTableMSSection;
    CoffRelocationTable        relocTableTSSection;
    CoffRelocationTable        relocTableTLSSection;
    CoffRelocationTable        relocTablePDSection;
    CoffRelocationTable        relocTableDBGSSection;
    Vector<CoffSymbol>         allSymbols;
    MapUtf8<uint32_t>          mapSymbols;
    MapUtf8<uint32_t>          globalStrings;
    Map<uint32_t, int32_t>     labels;
    DataSegment                globalSegment;
    DataSegment                stringSegment;
    VectorNative<LabelToSolve> labelsToSolve;
    Utf8                       directives;
    Vector<CoffFunction>       functions;

    uint32_t* patchSymbolTableOffset = nullptr;
    uint32_t* patchSymbolTableCount  = nullptr;
    uint32_t* patchTextSectionOffset = nullptr;
    uint32_t* patchTextSectionSize   = nullptr;

    uint32_t* patchTextSectionRelocTableOffset = nullptr;
    uint16_t* patchTextSectionRelocTableCount  = nullptr;
    uint32_t* patchTextSectionFlags            = nullptr;

    uint32_t* patchCSSectionRelocTableOffset = nullptr;
    uint16_t* patchCSSectionRelocTableCount  = nullptr;
    uint32_t* patchCSSectionFlags            = nullptr;

    uint32_t* patchMSSectionRelocTableOffset = nullptr;
    uint16_t* patchMSSectionRelocTableCount  = nullptr;
    uint32_t* patchMSSectionFlags            = nullptr;

    uint32_t* patchTLSSectionRelocTableOffset = nullptr;
    uint16_t* patchTLSSectionRelocTableCount  = nullptr;
    uint32_t* patchTLSSectionFlags            = nullptr;

    uint32_t* patchPDSectionRelocTableOffset = nullptr;
    uint16_t* patchPDSectionRelocTableCount  = nullptr;
    uint32_t* patchPDSectionFlags            = nullptr;

    uint32_t* patchDBGSSectionRelocTableOffset = nullptr;
    uint16_t* patchDBGSSectionRelocTableCount  = nullptr;
    uint32_t* patchDBGSSectionFlags            = nullptr;

    uint32_t* patchCSOffset   = nullptr;
    uint32_t* patchCSCount    = nullptr;
    uint32_t* patchSSOffset   = nullptr;
    uint32_t* patchSSCount    = nullptr;
    uint32_t* patchGSOffset   = nullptr;
    uint32_t* patchGSCount    = nullptr;
    uint32_t* patchMSOffset   = nullptr;
    uint32_t* patchMSCount    = nullptr;
    uint32_t* patchDRCount    = nullptr;
    uint32_t* patchDROffset   = nullptr;
    uint32_t* patchPDCount    = nullptr;
    uint32_t* patchPDOffset   = nullptr;
    uint32_t* patchXDCount    = nullptr;
    uint32_t* patchXDOffset   = nullptr;
    uint32_t* patchDBGSCount  = nullptr;
    uint32_t* patchDBGSOffset = nullptr;
    uint32_t* patchDBGTCount  = nullptr;
    uint32_t* patchDBGTOffset = nullptr;
    uint32_t* patchTLSOffset  = nullptr;
    uint32_t* patchTLSCount   = nullptr;

    uint32_t symCOIndex  = 0;
    uint32_t symBSIndex  = 0;
    uint32_t symMSIndex  = 0;
    uint32_t symCSIndex  = 0;
    uint32_t symTLSIndex = 0;
    uint32_t symXDIndex  = 0;

    uint32_t symMC_mainContext                  = 0;
    uint32_t symMC_mainContext_allocator_addr   = 0;
    uint32_t symMC_mainContext_allocator_itable = 0;
    uint32_t symMC_mainContext_flags            = 0;
    uint32_t symDefaultAllocTable               = 0;
    uint32_t symTls_threadLocalId               = 0;
    uint32_t symPI_processInfos                 = 0;
    uint32_t symPI_modulesAddr                  = 0;
    uint32_t symPI_modulesCount                 = 0;
    uint32_t symPI_argsAddr                     = 0;
    uint32_t symPI_argsCount                    = 0;
    uint32_t symPI_contextTlsId                 = 0;
    uint32_t symPI_defaultContext               = 0;
    uint32_t symPI_byteCodeRun                  = 0;
    uint32_t symPI_makeCallback                 = 0;
    uint32_t symPI_backendKind                  = 0;
    uint32_t symCst_U64F64                      = 0;

    uint32_t textSectionOffset = 0;
    uint32_t stringTableOffset = 0;

    uint16_t sectionIndexText = 0;
    uint16_t sectionIndexBS   = 0;
    uint16_t sectionIndexMS   = 0;
    uint16_t sectionIndexGS   = 0;
    uint16_t sectionIndexCS   = 0;
    uint16_t sectionIndexSS   = 0;
    uint16_t sectionIndexTS   = 0;
    uint16_t sectionIndexTLS  = 0;
    uint16_t sectionIndexDR   = 0;
    uint16_t sectionIndexPD   = 0;
    uint16_t sectionIndexXD   = 0;
    uint16_t sectionIndexDBGS = 0;
    uint16_t sectionIndexDBGT = 0;

    BackendPreCompilePass pass = {BackendPreCompilePass::Init};

    // Debug infos
    static const int             MAX_RECORD   = 4;
    uint16_t                     dbgRecordIdx = 0;
    uint16_t*                    dbgStartRecordPtr[MAX_RECORD];
    uint32_t                     dbgStartRecordOffset[MAX_RECORD];
    uint32_t                     dbgTypeRecordsCount = 0;
    Concat                       dbgTypeRecords;
    Map<TypeInfo*, DbgTypeIndex> dbgMapTypes;
    MapUtf8<DbgTypeIndex>        dbgMapPtrTypes;
    MapUtf8<DbgTypeIndex>        dbgMapRefTypes;
    MapUtf8<DbgTypeIndex>        dbgMapPtrPtrTypes;
    MapUtf8<DbgTypeIndex>        dbgMapTypesNames;

    uint32_t    storageRegCount = UINT32_MAX;
    uint32_t    storageRegStack = 0;
    uint32_t    storageRegBits  = 0;
    CPURegister storageReg      = RAX;
    CPURegister storageMemReg   = RAX;

    void clearInstructionCache();

    uint8_t     getREX(bool w = true, bool r = false, bool x = false, bool b = false);
    uint8_t     getModRM(uint8_t mod, uint8_t r, uint8_t m);
    CoffSymbol* getSymbol(const Utf8& name);
    CoffSymbol* getOrAddSymbol(const Utf8& name, CoffSymbolKind kind, uint32_t value = 0, uint16_t sectionIdx = 0);

    void emit_Add32_RSP(uint32_t value);
    void emit_Add64_Immediate(uint64_t value, CPURegister reg);
    void emit_Add64_RAX(uint64_t value);
    void emit_Add64_RCX(uint64_t value);
    void emit_Call_Indirect(CPURegister reg);
    void emit_Call_Parameters(TypeInfoFuncAttr* typeFunc, const VectorNative<uint32_t>& pushRAParams, uint32_t offsetRT, void* retCopy = nullptr);
    void emit_Call_Parameters(TypeInfoFuncAttr* typeFunc, const VectorNative<X64PushParam>& pushRAParams, uint32_t offsetRT, void* retCopy = nullptr);
    void emit_Call_Parameters(TypeInfoFuncAttr* typeFunc, VectorNative<X64PushParam>& paramsRegisters, VectorNative<TypeInfo*>& paramsTypes, void* retCopy = nullptr);
    void emit_Call_Result(TypeInfoFuncAttr* typeFunc, uint32_t offsetRT);
    void emit_Clear16(CPURegister reg);
    void emit_Clear32(CPURegister reg);
    void emit_Clear64(CPURegister reg);
    void emit_Clear8(CPURegister reg);
    void emit_ClearN(CPURegister reg, uint8_t numBits);
    void emit_ClearX(uint32_t count, uint32_t offset, CPURegister reg);
    void emit_Cmp16(uint8_t reg1, uint8_t reg2);
    void emit_Cmp16_Indirect(uint32_t offsetStack, CPURegister reg, CPURegister memReg);
    void emit_Cmp16_IndirectDst(uint32_t offsetStack, uint32_t value);
    void emit_Cmp32(uint8_t reg1, uint8_t reg2);
    void emit_Cmp32_Indirect(uint32_t offsetStack, CPURegister reg, CPURegister memReg);
    void emit_Cmp32_IndirectDst(uint32_t offsetStack, uint32_t value);
    void emit_Cmp64(uint8_t reg1, uint8_t reg2);
    void emit_Cmp64_Immediate(uint64_t value, CPURegister reg);
    void emit_Cmp64_Indirect(uint32_t offsetStack, CPURegister reg, CPURegister memReg);
    void emit_Cmp64_IndirectDst(uint32_t offsetStack, uint32_t value);
    void emit_Cmp8(uint8_t reg1, uint8_t reg2);
    void emit_Cmp8_Indirect(uint32_t offsetStack, CPURegister reg, CPURegister memReg);
    void emit_Cmp8_IndirectDst(uint32_t offsetStack, uint32_t value);
    void emit_CmpF32(uint8_t reg1, uint8_t reg2);
    void emit_CmpF32_Indirect(uint32_t offsetStack, CPURegister reg, CPURegister memReg);
    void emit_CmpF64(uint8_t reg1, uint8_t reg2);
    void emit_CmpF64_Indirect(uint32_t offsetStack, CPURegister reg, CPURegister memReg);
    void emit_Copy8(CPURegister regSrc, CPURegister regDst);
    void emit_Copy16(CPURegister regSrc, CPURegister regDst);
    void emit_Copy32(CPURegister regSrc, CPURegister regDst);
    void emit_Copy64(CPURegister regSrc, CPURegister regDst);
    void emit_CopyF32(CPURegister regSrc, CPURegister regDst);
    void emit_CopyF64(CPURegister regSrc, CPURegister regDst);
    void emit_CopyX(uint32_t count, uint32_t offset, CPURegister regDst, CPURegister regSrc);
    void emit_Extend_S8S16(CPURegister reg);
    void emit_Extend_S8S32(CPURegister reg);
    void emit_Extend_U8U32(CPURegister reg);
    void emit_Extend_U16U32(CPURegister reg);
    void emit_Extend_U16U64(CPURegister regSrc, CPURegister regDst);
    void emit_Extend_U8U64(CPURegister regSrc, CPURegister regDst);
    void emit_GlobalString(const Utf8& str, CPURegister reg);
    void emit_Jump(JumpType jumpType, int32_t instructionCount, int32_t jumpOffset);
    void emit_Load16_Immediate(uint16_t val, CPURegister reg);
    void emit_Load16_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg = RDI);
    void emit_Load32_Immediate(uint32_t val, CPURegister reg);
    void emit_Load32_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg = RDI);
    void emit_Load64_Immediate(uint64_t val, CPURegister reg, bool force64bits = false);
    void emit_Load64_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg = RDI);
    void emit_Load8_Immediate(uint8_t val, CPURegister reg);
    void emit_Load8_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg = RDI);
    void emit_LoadAddress_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg);
    void emit_LoadF32_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg = RDI);
    void emit_LoadF64_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg = RDI);
    void emit_LoadN_Immediate(Register& val, CPURegister reg, uint8_t numBits);
    void emit_LoadN_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg, uint8_t numBits);
    void emit_LoadS16S32_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg);
    void emit_LoadS16S64_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg);
    void emit_LoadS32S64_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg);
    void emit_LoadS8S16_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg);
    void emit_LoadS8S32_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg);
    void emit_LoadS8S64_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg);
    void emit_LoadU16U32_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg);
    void emit_LoadU16U64_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg);
    void emit_LoadU8U32_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg);
    void emit_LoadU8U64_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg);
    void emit_LongJumpOp(JumpType jumpType);
    void emit_REX();
    void emit_ModRM(uint32_t stackOffset, uint8_t reg, uint8_t memReg, uint8_t op = 1);
    void emit_Mul64_RAX(uint64_t value);
    void emit_NearJumpOp(JumpType jumpType);
    void emit_Op16(uint8_t reg1, uint8_t reg2, X64Op instruction);
    void emit_Op16_Indirect(uint32_t offsetStack, CPURegister reg, CPURegister memReg, X64Op instruction, bool lock = false);
    void emit_Op32(uint8_t reg1, uint8_t reg2, X64Op instruction);
    void emit_Op32_Indirect(uint32_t offsetStack, CPURegister reg, CPURegister memReg, X64Op instruction, bool lock = false);
    void emit_Op64(uint8_t reg1, uint8_t reg2, X64Op instruction);
    void emit_Op64_Indirect(uint32_t offsetStack, CPURegister reg, CPURegister memReg, X64Op instruction, bool lock = false);
    void emit_Op8(uint8_t reg1, uint8_t reg2, X64Op instruction);
    void emit_Op8_Indirect(uint32_t offsetStack, CPURegister reg, CPURegister memReg, X64Op instruction, bool lock = false);
    void emit_OpF32_Indirect(CPURegister reg, CPURegister memReg, X64Op instruction);
    void emit_OpF64_Indirect(CPURegister reg, CPURegister memReg, X64Op instruction);
    void emit_Pop(CPURegister reg);
    void emit_Push(CPURegister reg);
    void emit_Ret();
    void emit_SetA(CPURegister reg = RAX);
    void emit_SetAE(CPURegister reg = RAX);
    void emit_SetB();
    void emit_SetBE();
    void emit_SetE();
    void emit_SetEP();
    void emit_SetG();
    void emit_SetGE();
    void emit_SetL();
    void emit_SetLE();
    void emit_SetNA();
    void emit_SetNE();
    void emit_SetNEP();
    void emit_Store8_Immediate(uint32_t stackOffset, uint8_t val, CPURegister memReg);
    void emit_Store8_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg = RDI);
    void emit_Store16_Immediate(uint32_t stackOffset, uint16_t val, CPURegister memReg);
    void emit_Store16_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg = RDI);
    void emit_Store32_Immediate(uint32_t stackOffset, uint32_t val, CPURegister memReg);
    void emit_Store32_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg = RDI);
    void emit_Store64_Immediate(uint32_t stackOffset, uint64_t val, CPURegister memReg);
    void emit_Store64_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg = RDI);
    void emit_StoreF32_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg = RDI);
    void emit_StoreF64_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg = RDI);
    void emit_StoreN_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg, uint8_t numBits);
    void emit_Sub32_RSP(uint32_t value);
    void emit_Sub64_RAX(uint64_t value);
    void emit_Symbol_RelocationAddr(CPURegister reg, uint32_t symbolIndex, uint32_t offset);
    void emit_Symbol_RelocationValue(CPURegister reg, uint32_t symbolIndex, uint32_t offset);
    void emit_Test16(uint8_t reg1, uint8_t reg2);
    void emit_Test32(uint8_t reg1, uint8_t reg2);
    void emit_Test64(uint8_t reg1, uint8_t reg2);
    void emit_Test8(uint8_t reg1, uint8_t reg2);
    void emit_Cwd();
    void emit_Cdq();
    void emit_Cqo();
    void emit_Not8(CPURegister reg);
    void emit_Not16(CPURegister reg);
    void emit_Not32(CPURegister reg);
    void emit_Not64(CPURegister reg);
    void emit_Inc8_Indirect(uint32_t stackOffset, CPURegister memReg);
    void emit_Inc16_Indirect(uint32_t stackOffset, CPURegister memReg);
    void emit_Inc32_Indirect(uint32_t stackOffset, CPURegister memReg);
    void emit_Inc64_Indirect(uint32_t stackOffset, CPURegister memReg);
    void emit_Dec8_Indirect(uint32_t stackOffset, CPURegister memReg);
    void emit_Dec16_Indirect(uint32_t stackOffset, CPURegister memReg);
    void emit_Dec32_Indirect(uint32_t stackOffset, CPURegister memReg);
    void emit_Dec64_Indirect(uint32_t stackOffset, CPURegister memReg);
    void emit_Neg32(CPURegister reg);
    void emit_Neg64(CPURegister reg);
};
