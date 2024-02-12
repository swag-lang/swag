#pragma once
#include "Backend.h"

struct ByteCodeInstruction;
struct CPUFunction;
struct SCBE_CPU;
struct SCBE;

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
    SCBEDebugTypeRecordArgList     LF_ArgList;
    SCBEDebugTypeRecordProcedure   LF_Procedure;
    SCBEDebugTypeRecordMFunction   LF_MFunction;
    SCBEDebugTypeRecordFuncId      LF_FuncId;
    SCBEDebugTypeRecordMFuncId     LF_MFuncId;
    SCBEDebugTypeRecordFieldList   LF_FieldList;
    SCBEDebugTypeRecordDerivedList LF_DerivedList;
    SCBEDebugTypeRecordStructure   LF_Structure;
    SCBEDebugTypeRecordArray       LF_Array;
    SCBEDebugTypeRecordPointer     LF_Pointer;
    SCBEDebugTypeRecordEnum        LF_Enum;
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

struct SCBEDebug
{
    static void                 getStructFields(SCBE_CPU& pp, SCBEDebugTypeRecord* tr, TypeInfoStruct* typeStruct, uint32_t baseOffset);
    static SCBEDebugTypeIndex   getTypeSlice(SCBE_CPU& pp, const TypeInfo* typeInfo, TypeInfo* pointedType, SCBEDebugTypeIndex* value);
    static SCBEDebugTypeIndex   getSimpleType(const TypeInfo* typeInfo);
    static SCBEDebugTypeIndex   getOrCreatePointerToType(SCBE_CPU& pp, TypeInfo* typeInfo, bool asRef);
    static SCBEDebugTypeIndex   getOrCreatePointerPointerToType(SCBE_CPU& pp, TypeInfo* typeInfo);
    static SCBEDebugTypeIndex   getOrCreateType(SCBE_CPU& pp, TypeInfo* typeInfo, bool forceUnRef = false);
    static SCBEDebugTypeRecord* addTypeRecord(SCBE_CPU& pp);
    static void                 setLocation(CPUFunction* cpuFct, const ByteCode* bc, const ByteCodeInstruction* ip, uint32_t byteOffset);
    static Utf8                 getScopedName(AstNode* node);
};
