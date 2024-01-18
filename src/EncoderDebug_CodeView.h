#pragma once

// https://github.com/microsoft/microsoft-pdb/blob/master/include/cvinfo.h

const uint32_t DEBUG_SECTION_MAGIC = 4;

const uint32_t DEBUG_S_SYMBOLS      = 0xF1; // DebugSubsectionKind::Symbols in llvm
const uint32_t DEBUG_S_LINES        = 0xF2;
const uint32_t DEBUG_S_STRINGTABLE  = 0xF3;
const uint32_t DEBUG_S_FILECHKSMS   = 0xF4;
const uint32_t DEBUG_S_INLINEELINES = 0xF6;

const uint16_t S_END                   = 0x0006;
const uint16_t S_FRAMEPROC             = 0x1012;
const uint16_t S_BLOCK32               = 0x1103;
const uint16_t S_COMPILE3              = 0x113c;
const uint16_t S_LPROC32_ID            = 0x1146;
const uint16_t S_PROC_ID_END           = 0x114F;
const uint16_t S_LOCAL                 = 0x113E;
const uint16_t S_DEFRANGE_REGISTER_REL = 0x1145;
const uint16_t S_CONSTANT              = 0x1107;
const uint16_t S_LDATA32               = 0x110C;
// const uint16_t S_INLINESITE            = 0x114d;
// const uint16_t S_INLINESITE_END        = 0x114e;
// const uint16_t S_GPROC32_ID                = 0x1147;
// const uint16_t S_DEFRANGE                  = 0x113F;
// const uint16_t S_DEFRANGE_REGISTER         = 0x1141;
// const uint16_t S_DEFRANGE_FRAMEPOINTER_REL = 0x1142;

const uint16_t LF_POINTER   = 0x1002;
const uint16_t LF_PROCEDURE = 0x1008;
const uint16_t LF_MFUNCTION = 0x1009;
const uint16_t LF_ARGLIST   = 0x1201;
const uint16_t LF_FIELDLIST = 0x1203;
const uint16_t LF_DERIVED   = 0x1204;
const uint16_t LF_ENUMERATE = 0x1502;
const uint16_t LF_ARRAY     = 0x1503;
const uint16_t LF_STRUCTURE = 0x1505;
const uint16_t LF_ENUM      = 0x1507;
const uint16_t LF_MEMBER    = 0x150d;
const uint16_t LF_ONEMETHOD = 0x1511;
const uint16_t LF_FUNC_ID   = 0x1601;
const uint16_t LF_MFUNC_ID  = 0x1602;
// const uint16_t LF_METHOD       = 0x150f;
// const uint16_t LF_UDT_SRC_LINE = 0x1606;

// const uint16_t LF_NUMERIC   = 0x8000;
const uint16_t LF_CHAR      = 0x8000;
const uint16_t LF_SHORT     = 0x8001;
const uint16_t LF_USHORT    = 0x8002;
const uint16_t LF_LONG      = 0x8003;
const uint16_t LF_ULONG     = 0x8004;
const uint16_t LF_REAL32    = 0x8005;
const uint16_t LF_REAL64    = 0x8006;
const uint16_t LF_QUADWORD  = 0x8009;
const uint16_t LF_UQUADWORD = 0x800a;
// const uint16_t LF_REAL80    = 0x8007;
// const uint16_t LF_REAL128   = 0x8008;

const uint8_t CV_PTR_MODE_LVREF = 0x01; // l-value reference
// const uint8_t CV_PTR_MODE_PTR      = 0x00; // "normal" pointer
// const uint8_t CV_PTR_MODE_REF      = 0x01; // "old" reference
// const uint8_t CV_PTR_MODE_PMEM     = 0x02; // pointer to data member
// const uint8_t CV_PTR_MODE_PMFUNC   = 0x03; // pointer to member function
// const uint8_t CV_PTR_MODE_RVREF    = 0x04; // r-value reference
// const uint8_t CV_PTR_MODE_RESERVED = 0x05; // first unused pointer mode

// https://github.com/microsoft/checkedc-llvm/blob/master/include/llvm/DebugInfo/CodeView/CodeViewRegisters.def
const uint16_t R_RDI = 333;
// const uint16_t R_R11 = 339;
const uint16_t R_R12 = 340;
// const uint16_t R_RCX = 330;
// const uint16_t R_RDX = 331;
// const uint16_t R_RBP = 334;
// const uint16_t R_RSP = 335;

enum SimpleTypeKind : DbgTypeIndex
{
    None          = 0x0000, // uncharacterized type (no type)
    Void          = 0x0003, // void
    NotTranslated = 0x0007, // type not translated by cvpack
    HResult       = 0x0008, // OLE/COM HRESULT

    SignedCharacter   = 0x0010, // 8 bit signed
    UnsignedCharacter = 0x0020, // 8 bit unsigned
    NarrowCharacter   = 0x0070, // really a char
    WideCharacter     = 0x0071, // wide char
    Character16       = 0x007a, // uint16_t
    Character32       = 0x007b, // uint32_t

    SByte       = 0x0068, // 8 bit signed int
    Byte        = 0x0069, // 8 bit unsigned int
    Int16Short  = 0x0011, // 16 bit signed
    UInt16Short = 0x0021, // 16 bit unsigned
    Int16       = 0x0072, // 16 bit signed int
    UInt16      = 0x0073, // 16 bit unsigned int
    Int32Long   = 0x0012, // 32 bit signed
    UInt32Long  = 0x0022, // 32 bit unsigned
    Int32       = 0x0074, // 32 bit signed int
    UInt32      = 0x0075, // 32 bit unsigned int
    Int64Quad   = 0x0013, // 64 bit signed
    UInt64Quad  = 0x0023, // 64 bit unsigned
    Int64       = 0x0076, // 64 bit signed int
    UInt64      = 0x0077, // 64 bit unsigned int
    Int128Oct   = 0x0014, // 128 bit signed int
    UInt128Oct  = 0x0024, // 128 bit unsigned int
    Int128      = 0x0078, // 128 bit signed int
    UInt128     = 0x0079, // 128 bit unsigned int

    Float16                 = 0x0046, // 16 bit real
    Float32                 = 0x0040, // 32 bit real
    Float32PartialPrecision = 0x0045, // 32 bit PP real
    Float48                 = 0x0044, // 48 bit real
    Float64                 = 0x0041, // 64 bit real
    Float80                 = 0x0042, // 80 bit real
    Float128                = 0x0043, // 128 bit real

    Complex16                 = 0x0056, // 16 bit complex
    Complex32                 = 0x0050, // 32 bit complex
    Complex32PartialPrecision = 0x0055, // 32 bit PP complex
    Complex48                 = 0x0054, // 48 bit complex
    Complex64                 = 0x0051, // 64 bit complex
    Complex80                 = 0x0052, // 80 bit complex
    Complex128                = 0x0053, // 128 bit complex

    Boolean8   = 0x0030, // 8 bit boolean
    Boolean16  = 0x0031, // 16 bit boolean
    Boolean32  = 0x0032, // 32 bit boolean
    Boolean64  = 0x0033, // 64 bit boolean
    Boolean128 = 0x0034, // 128 bit boolean
};

enum SimpleTypeMode : DbgTypeIndex
{
    Direct         = 0, // Not a pointer
    NearPointer    = 1, // Near pointer
    FarPointer     = 2, // Far pointer
    HugePointer    = 3, // Huge pointer
    NearPointer32  = 4, // 32 bit near pointer
    FarPointer32   = 5, // 32 bit far pointer
    NearPointer64  = 6, // 64 bit near pointer
    NearPointer128 = 7  // 128 bit near pointer
};

struct EncoderDebug_CodeView
{
    void startRecord(EncoderCPU& pp, uint16_t what);
    void endRecord(EncoderCPU& pp, bool align = true);
    void emitEmbeddedValue(EncoderCPU& pp, TypeInfo* valueType, ComputedValue& val);
    void emitSecRel(EncoderCPU& pp, uint32_t symbolIndex, uint32_t segIndex, uint32_t offset = 0);
    void emitTruncatedString(EncoderCPU& pp, const Utf8& str);
    void emitCompilerFlagsDebugS(EncoderCPU& pp);
    void emitConstant(EncoderCPU& pp, AstNode* node, const Utf8& name);
    void emitGlobalDebugS(EncoderCPU& pp, VectorNative<AstNode*>& gVars, uint32_t segSymIndex);
    bool emitDataDebugT(EncoderCPU& pp);
    bool emitLines(EncoderCPU& pp, MapPath<uint32_t>&, Vector<uint32_t>&, Utf8&, Concat& concat, CoffFunction& f, size_t idxDbgLines);
    bool emitFctDebugS(EncoderCPU& pp);
    bool emitScope(EncoderCPU& pp, CoffFunction& f, Scope* scope);
    bool emit(const BuildParameters& buildParameters, EncoderCPU& pp);

    BackendSCBE* scbe = nullptr;
};