#pragma once
#include "Backend.h"
#include "BackendParameters.h"
#include "DataSegment.h"

struct BuildParameters;
struct ByteCode;
struct ByteCodeInstruction;
struct CoffFunction;
struct DataSegment;
struct EncoderCPU;
struct Job;
struct Module;
struct TypeInfo;
struct BackendSCBE;

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
    Vector<DbgLine> lines;
    AstFuncDecl*    inlined = nullptr;
};

struct BackendSCBEDbg
{
    static void dbgSetLocation(CoffFunction* coffFct, ByteCode* bc, ByteCodeInstruction* ip, uint32_t byteOffset);
    static Utf8 dbgGetScopedName(AstNode* node);
    static bool dbgEmit(const BuildParameters& buildParameters, BackendSCBE* scbe, EncoderCPU& pp);
};