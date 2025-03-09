#pragma once
#include "Backend/Backend.h"
#include "Syntax/ComputedValue.h"

struct ByteCodeInstruction;
struct CPUFunction;
struct SCBECPU;
struct SCBE;
struct SourceFile;
struct AstFuncDecl;
struct TypeInfoStruct;
struct ByteCode;

using SCBEDebugTypeIndex = uint32_t;

struct SCBEDebugTypeRecordArgList
{
    Vector<SCBEDebugTypeIndex> args;
    uint32_t                   count = 0;
};

struct SCBEDebugTypeRecordProcedure
{
    SCBEDebugTypeIndex returnType = 0;
    SCBEDebugTypeIndex argsType   = 0;
    uint16_t           numArgs    = 0;
};

struct SCBEDebugTypeRecordMFunction
{
    SCBEDebugTypeIndex returnType = 0;
    SCBEDebugTypeIndex structType = 0;
    SCBEDebugTypeIndex thisType   = 0;
    SCBEDebugTypeIndex argsType   = 0;
    uint16_t           numArgs    = 0;
};

struct SCBEDebugTypeRecordFuncId
{
    SCBEDebugTypeIndex type = 0;
};

struct SCBEDebugTypeRecordMFuncId
{
    SCBEDebugTypeIndex parentType = 0;
    SCBEDebugTypeIndex type       = 0;
};

struct SCBEDebugTypeField
{
    Utf8               name;
    ComputedValue      value;
    TypeInfo*          valueType       = nullptr;
    SCBEDebugTypeIndex type            = 0;
    uint16_t           accessSpecifier = 0;
    uint16_t           kind            = 0;
};

struct SCBEDebugTypeRecordFieldList
{
    Vector<SCBEDebugTypeField> fields;
};

struct SCBEDebugTypeRecordDerivedList
{
    Vector<SCBEDebugTypeIndex> derived;
};

struct SCBEDebugTypeRecordStructure
{
    SCBEDebugTypeIndex fieldList   = 0;
    SCBEDebugTypeIndex derivedList = 0;
    uint32_t           sizeOf      = 0;
    uint16_t           memberCount = 0;
    bool               forward     = false;
};

struct SCBEDebugTypeRecordEnum
{
    SCBEDebugTypeIndex fieldList      = 0;
    SCBEDebugTypeIndex underlyingType = 0;
    uint16_t           count          = 0;
};

struct SCBEDebugTypeRecordArray
{
    SCBEDebugTypeIndex elementType = 0;
    SCBEDebugTypeIndex indexType   = 0;
    uint32_t           sizeOf      = 0;
};

struct SCBEDebugTypeRecordPointer
{
    SCBEDebugTypeIndex pointeeType = 0;
    bool               asRef       = false;
};

struct SCBEDebugTypeRecord
{
    Utf8                           name;
    AstNode*                       node = nullptr;
    SCBEDebugTypeRecordArgList     lfArgList;
    SCBEDebugTypeRecordProcedure   lfProcedure;
    SCBEDebugTypeRecordMFunction   lfMFunction;
    SCBEDebugTypeRecordFuncId      lfFuncId;
    SCBEDebugTypeRecordMFuncId     lfMFuncId;
    SCBEDebugTypeRecordFieldList   lfFieldList;
    SCBEDebugTypeRecordDerivedList lfDerivedList;
    SCBEDebugTypeRecordStructure   lfStructure;
    SCBEDebugTypeRecordArray       lfArray;
    SCBEDebugTypeRecordPointer     lfPointer;
    SCBEDebugTypeRecordEnum        lfEnum;
    SCBEDebugTypeIndex             index = 0;
    uint16_t                       kind  = 0;
};

struct SCBEDebugLine
{
    uint32_t line;
    uint32_t byteOffset;
};

struct SCBEDebugLines
{
    SourceFile*           sourceFile;
    Vector<SCBEDebugLine> lines;
    AstFuncDecl*          inlined = nullptr;
};

struct SCBE_Debug
{
    static void                 getStructFields(SCBECPU& pp, SCBEDebugTypeRecord* tr, TypeInfoStruct* typeStruct, uint32_t baseOffset);
    static SCBEDebugTypeIndex   getTypeSlice(SCBECPU& pp, const TypeInfo* typeInfo, TypeInfo* pointedType, SCBEDebugTypeIndex* value);
    static SCBEDebugTypeIndex   getSimpleType(const TypeInfo* typeInfo);
    static SCBEDebugTypeIndex   getOrCreatePointerToType(SCBECPU& pp, TypeInfo* typeInfo, bool asRef);
    static SCBEDebugTypeIndex   getOrCreatePointerPointerToType(SCBECPU& pp, TypeInfo* typeInfo);
    static SCBEDebugTypeIndex   getOrCreateType(SCBECPU& pp, TypeInfo* typeInfo, bool forceUnRef = false);
    static SCBEDebugTypeRecord* addTypeRecord(SCBECPU& pp);
    static void                 setLocation(CPUFunction* cpuFct, const ByteCodeInstruction* ip, uint32_t byteOffset);
    static Utf8                 getScopedName(const AstNode* node);
};
