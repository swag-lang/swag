#pragma once
struct TypeInfo;
struct TypeInfoStruct;
struct TypeInfoEnum;
struct Job;

struct ScopeSwag
{
    void registerType(TypeInfo* typeInfo);

    TypeInfoStruct* regTypeInfo           = nullptr;
    TypeInfoStruct* regTypeInfoNative     = nullptr;
    TypeInfoStruct* regTypeInfoPointer    = nullptr;
    TypeInfoStruct* regTypeInfoStruct     = nullptr;
    TypeInfoStruct* regTypeInfoList       = nullptr;
    TypeInfoStruct* regTypeInfoParam      = nullptr;
    TypeInfoStruct* regTypeInfoFunc       = nullptr;
    TypeInfoStruct* regTypeInfoEnum       = nullptr;
    TypeInfoStruct* regTypeInfoVariadic   = nullptr;
    TypeInfoStruct* regTypeInfoArray      = nullptr;
    TypeInfoStruct* regTypeInfoSlice      = nullptr;
    TypeInfoStruct* regTypeInfoGeneric    = nullptr;
    TypeInfoStruct* regTypeInfoCode       = nullptr;
    TypeInfoStruct* regTypeInfoAlias      = nullptr;
    TypeInfoStruct* regTypeInfoNamespace  = nullptr;
    TypeInfoStruct* regTypeInfoSourceLoc  = nullptr;
    TypeInfoEnum*   regTypeInfoTargetArch = nullptr;
    TypeInfoEnum*   regTypeInfoTargetOs   = nullptr;
    TypeInfoEnum*   regTypeInfoBackend    = nullptr;
};
