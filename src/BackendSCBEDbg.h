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
    DbgTypeIndex   dbgEmitTypeSlice(EncoderCPU& pp, TypeInfo* typeInfo, TypeInfo* pointedType, DbgTypeIndex* value);
    void           dbgEmitEmbeddedValue(Concat& concat, TypeInfo* valueType, ComputedValue& val);
    void           dbgStartRecord(EncoderCPU& pp, Concat& concat, uint16_t what);
    void           dbgEndRecord(EncoderCPU& pp, Concat& concat, bool align = true);
    void           dbgEmitSecRel(EncoderCPU& pp, Concat& concat, uint32_t symbolIndex, uint32_t segIndex, uint32_t offset = 0);
    void           dbgEmitTruncatedString(Concat& concat, const Utf8& str);
    DbgTypeIndex   dbgGetSimpleType(TypeInfo* typeInfo);
    DbgTypeIndex   dbgGetOrCreatePointerToType(EncoderCPU& pp, TypeInfo* typeInfo, bool asRef);
    DbgTypeIndex   dbgGetOrCreatePointerPointerToType(EncoderCPU& pp, TypeInfo* typeInfo);
    void           dbgRecordFields(EncoderCPU& pp, DbgTypeRecord* tr, TypeInfoStruct* typeStruct, uint32_t baseOffset);
    DbgTypeIndex   dbgGetOrCreateType(EncoderCPU& pp, TypeInfo* typeInfo, bool forceUnRef = false);
    DbgTypeRecord* dbgAddTypeRecord(EncoderCPU& pp);
    Utf8           dbgGetScopedName(AstNode* node);
    void           dbgSetLocation(CoffFunction* coffFct, ByteCode* bc, ByteCodeInstruction* ip, uint32_t byteOffset);
    void           dbgEmitCompilerFlagsDebugS(Concat& concat);
    void           dbgEmitConstant(EncoderCPU& pp, Concat& concat, AstNode* node, const Utf8& name);
    void           dbgEmitGlobalDebugS(EncoderCPU& pp, Concat& concat, VectorNative<AstNode*>& gVars, uint32_t segSymIndex);
    bool           dbgEmitDataDebugT(EncoderCPU& pp);
    bool           dbgEmitLines(EncoderCPU& pp, MapPath<uint32_t>&, Vector<uint32_t>&, Utf8&, Concat& concat, CoffFunction& f, size_t idxDbgLines);
    bool           dbgEmitFctDebugS(EncoderCPU& pp);
    bool           dbgEmitScope(EncoderCPU& pp, Concat& concat, CoffFunction& f, Scope* scope);
    bool           dbgEmit(const BuildParameters& buildParameters, EncoderCPU& pp);

    BackendSCBE* scbe = nullptr;
};