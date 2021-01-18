#pragma once
struct TypeInfo;
struct TypeInfoStruct;
struct Job;

struct SwagScope
{
    void registerType(TypeInfo* typeInfo);

    TypeInfoStruct* regTypeInfo          = nullptr;
    TypeInfoStruct* regTypeInfoNative    = nullptr;
    TypeInfoStruct* regTypeInfoPointer   = nullptr;
    TypeInfoStruct* regTypeInfoReference = nullptr;
    TypeInfoStruct* regTypeInfoStruct    = nullptr;
    TypeInfoStruct* regTypeInfoList      = nullptr;
    TypeInfoStruct* regTypeInfoParam     = nullptr;
    TypeInfoStruct* regTypeInfoFunc      = nullptr;
    TypeInfoStruct* regTypeInfoEnum      = nullptr;
    TypeInfoStruct* regTypeInfoVariadic  = nullptr;
    TypeInfoStruct* regTypeInfoArray     = nullptr;
    TypeInfoStruct* regTypeInfoSlice     = nullptr;
    TypeInfoStruct* regTypeInfoGeneric   = nullptr;
    TypeInfoStruct* regTypeInfoAlias     = nullptr;
    TypeInfoStruct* regTypeInfoSourceLoc = nullptr;
};
