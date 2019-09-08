#pragma once
#include "SpinLock.h"
struct TypeInfo;
struct TypeInfoStruct;
struct Job;

struct SwagScope
{
    void registerType(TypeInfo* typeInfo);

    TypeInfoStruct* regTypeInfo        = nullptr;
    TypeInfoStruct* regTypeInfoNative  = nullptr;
    TypeInfoStruct* regTypeInfoPointer = nullptr;
    TypeInfoStruct* regTypeInfoStruct  = nullptr;
    TypeInfoStruct* regTypeInfoParam   = nullptr;

    bool         fullySolved = false;
    uint32_t     cptSolved   = 0;
    SpinLock     mutex;
    vector<Job*> dependentJobs;
};
