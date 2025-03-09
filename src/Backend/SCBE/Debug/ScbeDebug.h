#pragma once
#include "Backend/Backend.h"
#include "Syntax/ComputedValue.h"

struct ByteCodeInstruction;
struct CPUFunction;
struct ScbeCPU;
struct Scbe;
struct SourceFile;
struct AstFuncDecl;
struct TypeInfoStruct;
struct ByteCode;

using ScbeDebugTypeIndex = uint32_t;

struct ScbeDebugTypeRecordArgList
{
    Vector<ScbeDebugTypeIndex> args;
    uint32_t                   count = 0;
};

struct ScbeDebugTypeRecordProcedure
{
    ScbeDebugTypeIndex returnType = 0;
    ScbeDebugTypeIndex argsType   = 0;
    uint16_t           numArgs    = 0;
};

struct ScbeDebugTypeRecordMFunction
{
    ScbeDebugTypeIndex returnType = 0;
    ScbeDebugTypeIndex structType = 0;
    ScbeDebugTypeIndex thisType   = 0;
    ScbeDebugTypeIndex argsType   = 0;
    uint16_t           numArgs    = 0;
};

struct ScbeDebugTypeRecordFuncId
{
    ScbeDebugTypeIndex type = 0;
};

struct ScbeDebugTypeRecordMFuncId
{
    ScbeDebugTypeIndex parentType = 0;
    ScbeDebugTypeIndex type       = 0;
};

struct ScbeDebugTypeField
{
    Utf8               name;
    ComputedValue      value;
    TypeInfo*          valueType       = nullptr;
    ScbeDebugTypeIndex type            = 0;
    uint16_t           accessSpecifier = 0;
    uint16_t           kind            = 0;
};

struct ScbeDebugTypeRecordFieldList
{
    Vector<ScbeDebugTypeField> fields;
};

struct ScbeDebugTypeRecordDerivedList
{
    Vector<ScbeDebugTypeIndex> derived;
};

struct ScbeDebugTypeRecordStructure
{
    ScbeDebugTypeIndex fieldList   = 0;
    ScbeDebugTypeIndex derivedList = 0;
    uint32_t           sizeOf      = 0;
    uint16_t           memberCount = 0;
    bool               forward     = false;
};

struct ScbeDebugTypeRecordEnum
{
    ScbeDebugTypeIndex fieldList      = 0;
    ScbeDebugTypeIndex underlyingType = 0;
    uint16_t           count          = 0;
};

struct ScbeDebugTypeRecordArray
{
    ScbeDebugTypeIndex elementType = 0;
    ScbeDebugTypeIndex indexType   = 0;
    uint32_t           sizeOf      = 0;
};

struct ScbeDebugTypeRecordPointer
{
    ScbeDebugTypeIndex pointeeType = 0;
    bool               asRef       = false;
};

struct ScbeDebugTypeRecord
{
    Utf8                           name;
    AstNode*                       node = nullptr;
    ScbeDebugTypeRecordArgList     lfArgList;
    ScbeDebugTypeRecordProcedure   lfProcedure;
    ScbeDebugTypeRecordMFunction   lfMFunction;
    ScbeDebugTypeRecordFuncId      lfFuncId;
    ScbeDebugTypeRecordMFuncId     lfMFuncId;
    ScbeDebugTypeRecordFieldList   lfFieldList;
    ScbeDebugTypeRecordDerivedList lfDerivedList;
    ScbeDebugTypeRecordStructure   lfStructure;
    ScbeDebugTypeRecordArray       lfArray;
    ScbeDebugTypeRecordPointer     lfPointer;
    ScbeDebugTypeRecordEnum        lfEnum;
    ScbeDebugTypeIndex             index = 0;
    uint16_t                       kind  = 0;
};

struct ScbeDebugLine
{
    uint32_t line;
    uint32_t byteOffset;
};

struct ScbeDebugLines
{
    SourceFile*           sourceFile;
    Vector<ScbeDebugLine> lines;
    AstFuncDecl*          inlined = nullptr;
};

struct ScbeDebug
{
    static void                 getStructFields(ScbeCPU& pp, ScbeDebugTypeRecord* tr, TypeInfoStruct* typeStruct, uint32_t baseOffset);
    static ScbeDebugTypeIndex   getTypeSlice(ScbeCPU& pp, const TypeInfo* typeInfo, TypeInfo* pointedType, ScbeDebugTypeIndex* value);
    static ScbeDebugTypeIndex   getSimpleType(const TypeInfo* typeInfo);
    static ScbeDebugTypeIndex   getOrCreatePointerToType(ScbeCPU& pp, TypeInfo* typeInfo, bool asRef);
    static ScbeDebugTypeIndex   getOrCreatePointerPointerToType(ScbeCPU& pp, TypeInfo* typeInfo);
    static ScbeDebugTypeIndex   getOrCreateType(ScbeCPU& pp, TypeInfo* typeInfo, bool forceUnRef = false);
    static ScbeDebugTypeRecord* addTypeRecord(ScbeCPU& pp);
    static void                 setLocation(CPUFunction* cpuFct, const ByteCodeInstruction* ip, uint32_t byteOffset);
    static Utf8                 getScopedName(const AstNode* node);
};
