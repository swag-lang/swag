#include "pch.h"
#include "LanguageSpec.h"

LanguageSpec::LanguageSpec()
{
    keywords["#unittest"] = TokenId::CompilerUnitTest;
    keywords["var"]       = TokenId::KwdVar;

#ifdef SWAG_TEST_CPP
    keywords["#include"] = TokenId::Invalid;
    keywords["#pragma"]  = TokenId::Invalid;
    keywords["#ifdef"]   = TokenId::Invalid;
    keywords["#ifndef"]  = TokenId::Invalid;
    keywords["#endif"]   = TokenId::Invalid;
    keywords["#elif"]    = TokenId::Invalid;
    keywords["#else"]    = TokenId::Invalid;
    keywords["#if"]      = TokenId::Invalid;
    keywords["#define"]  = TokenId::Invalid;
    keywords["#"]        = TokenId::Invalid;
    keywords["#undef"]   = TokenId::Invalid;
    keywords["#error"]   = TokenId::Invalid;
    keywords["#argname"] = TokenId::Invalid;
    keywords["#name"]    = TokenId::Invalid;
    keywords["#x"]       = TokenId::Invalid;
    keywords["#A"]       = TokenId::Invalid;
    keywords["#__val"]   = TokenId::Invalid;
    keywords["#proc"]    = TokenId::Invalid;
    keywords["#_FCT"]    = TokenId::Invalid;
    keywords["#Oper"]    = TokenId::Invalid;
    keywords["#__a"]     = TokenId::Invalid;
    keywords["#a"]       = TokenId::Invalid;
#endif

    keywords["u64"]  = TokenId::NativeType;
    keywords["u32"]  = TokenId::NativeType;
    keywords["u16"]  = TokenId::NativeType;
    keywords["u8"]   = TokenId::NativeType;
    keywords["s64"]  = TokenId::NativeType;
    keywords["s32"]  = TokenId::NativeType;
    keywords["s16"]  = TokenId::NativeType;
    keywords["s8"]   = TokenId::NativeType;
    keywords["f32"]  = TokenId::NativeType;
    keywords["f64"]  = TokenId::NativeType;
    keywords["bool"] = TokenId::NativeType;
    keywords["char"] = TokenId::NativeType;

    nativeTypes["u64"]  = TokenNumType::UInt64;
    nativeTypes["u32"]  = TokenNumType::UInt32;
    nativeTypes["u16"]  = TokenNumType::UInt16;
    nativeTypes["u8"]   = TokenNumType::UInt8;
    nativeTypes["s64"]  = TokenNumType::Int64;
    nativeTypes["s32"]  = TokenNumType::Int32;
    nativeTypes["s16"]  = TokenNumType::Int16;
    nativeTypes["s8"]   = TokenNumType::Int8;
    nativeTypes["f32"]  = TokenNumType::Float32;
    nativeTypes["f64"]  = TokenNumType::Float64;
    nativeTypes["bool"] = TokenNumType::Bool;
    nativeTypes["char"] = TokenNumType::Char;
}

string LanguageSpec::tokenToName(TokenId id)
{
    switch (id)
    {
    case TokenId::SymColon:
        return ":";
    case TokenId::SymSemiColon:
        return ";";
    }

    return "???";
}