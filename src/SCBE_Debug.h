#pragma once
#include "Backend.h"

struct ByteCodeInstruction;
struct CPUFunction;
struct SCBE_CPU;
struct SCBE;

using SCBE_DebugTypeIndex = uint32_t;

struct SCBE_DebugTypeRecordArgList
{
    Vector<SCBE_DebugTypeIndex> args;
    uint32_t                    count = 0;
};

struct SCBE_DebugTypeRecordProcedure
{
    SCBE_DebugTypeIndex returnType = 0;
    SCBE_DebugTypeIndex argsType   = 0;
    uint16_t            numArgs    = 0;
};

struct SCBE_DebugTypeRecordMFunction
{
    SCBE_DebugTypeIndex returnType = 0;
    SCBE_DebugTypeIndex structType = 0;
    SCBE_DebugTypeIndex thisType   = 0;
    SCBE_DebugTypeIndex argsType   = 0;
    uint16_t            numArgs    = 0;
};

struct SCBE_DebugTypeRecordFuncId
{
    SCBE_DebugTypeIndex type = 0;
};

struct SCBE_DebugTypeRecordMFuncId
{
    SCBE_DebugTypeIndex parentType = 0;
    SCBE_DebugTypeIndex type       = 0;
};

struct SCBE_DebugTypeField
{
    Utf8                name;
    ComputedValue       value;
    TypeInfo*           valueType       = nullptr;
    SCBE_DebugTypeIndex type            = 0;
    uint16_t            accessSpecifier = 0;
    uint16_t            kind            = 0;
};

struct SCBE_DebugTypeRecordFieldList
{
    Vector<SCBE_DebugTypeField> fields;
};

struct SCBE_DebugTypeRecordDerivedList
{
    Vector<SCBE_DebugTypeIndex> derived;
};

struct SCBE_DebugTypeRecordStructure
{
    SCBE_DebugTypeIndex fieldList   = 0;
    SCBE_DebugTypeIndex derivedList = 0;
    uint32_t            sizeOf      = 0;
    uint16_t            memberCount = 0;
    bool                forward     = false;
};

struct SCBE_DebugTypeRecordEnum
{
    SCBE_DebugTypeIndex fieldList      = 0;
    SCBE_DebugTypeIndex underlyingType = 0;
    uint16_t            count          = 0;
};

struct SCBE_DebugTypeRecordArray
{
    SCBE_DebugTypeIndex elementType = 0;
    SCBE_DebugTypeIndex indexType   = 0;
    uint32_t            sizeOf      = 0;
};

struct SCBE_DebugTypeRecordPointer
{
    SCBE_DebugTypeIndex pointeeType = 0;
    bool                asRef       = false;
};

struct SCBE_DebugTypeRecord
{
    Utf8                            name;
    AstNode*                        node = nullptr;
    SCBE_DebugTypeRecordArgList     LF_ArgList;
    SCBE_DebugTypeRecordProcedure   LF_Procedure;
    SCBE_DebugTypeRecordMFunction   LF_MFunction;
    SCBE_DebugTypeRecordFuncId      LF_FuncId;
    SCBE_DebugTypeRecordMFuncId     LF_MFuncId;
    SCBE_DebugTypeRecordFieldList   LF_FieldList;
    SCBE_DebugTypeRecordDerivedList LF_DerivedList;
    SCBE_DebugTypeRecordStructure   LF_Structure;
    SCBE_DebugTypeRecordArray       LF_Array;
    SCBE_DebugTypeRecordPointer     LF_Pointer;
    SCBE_DebugTypeRecordEnum        LF_Enum;
    SCBE_DebugTypeIndex             index = 0;
    uint16_t                        kind  = 0;
};

struct SCBE_DebugLine
{
    uint32_t line;
    uint32_t byteOffset;
};

struct SCBE_DebugLines
{
    SourceFile*            sourceFile;
    Vector<SCBE_DebugLine> lines;
    AstFuncDecl*           inlined = nullptr;
};

struct SCBE_Debug
{
    static void                  getStructFields(SCBE_CPU& pp, SCBE_DebugTypeRecord* tr, TypeInfoStruct* typeStruct, uint32_t baseOffset);
    static SCBE_DebugTypeIndex   getTypeSlice(SCBE_CPU& pp, TypeInfo* typeInfo, TypeInfo* pointedType, SCBE_DebugTypeIndex* value);
    static SCBE_DebugTypeIndex   getSimpleType(TypeInfo* typeInfo);
    static SCBE_DebugTypeIndex   getOrCreatePointerToType(SCBE_CPU& pp, TypeInfo* typeInfo, bool asRef);
    static SCBE_DebugTypeIndex   getOrCreatePointerPointerToType(SCBE_CPU& pp, TypeInfo* typeInfo);
    static SCBE_DebugTypeIndex   getOrCreateType(SCBE_CPU& pp, TypeInfo* typeInfo, bool forceUnRef = false);
    static SCBE_DebugTypeRecord* addTypeRecord(SCBE_CPU& pp);
    static void                  setLocation(CPUFunction* cpuFct, ByteCode* bc, ByteCodeInstruction* ip, uint32_t byteOffset);
    static Utf8                  getScopedName(AstNode* node);
};
