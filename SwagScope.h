#pragma once
struct TypeInfo;
struct TypeInfoStruct;
struct Job;

struct SwagScope
{
    void registerType(TypeInfo* typeInfo);

    TypeInfoStruct* regTypeInfo         = nullptr;
    TypeInfoStruct* regTypeInfoNative   = nullptr;
    TypeInfoStruct* regTypeInfoPointer  = nullptr;
    TypeInfoStruct* regTypeInfoStruct   = nullptr;
    TypeInfoStruct* regTypeInfoParam    = nullptr;
    TypeInfoStruct* regTypeInfoFunc     = nullptr;
    TypeInfoStruct* regTypeInfoEnum     = nullptr;
    TypeInfoStruct* regTypeInfoVariadic = nullptr;
    TypeInfoStruct* regTypeInfoArray    = nullptr;
    TypeInfoStruct* regTypeInfoSlice    = nullptr;
};
