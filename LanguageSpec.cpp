#include "pch.h"
#include "LanguageSpec.h"

LanguageSpec::LanguageSpec()
{
    keywords["#unittest"] = TokenId::CompilerUnitTest;
    keywords["#assert"] = TokenId::CompilerAssert;
	keywords["#print"] = TokenId::CompilerPrint;
	keywords["#run"] = TokenId::CompilerRun;
    keywords["var"]       = TokenId::KwdVar;
    keywords["namespace"] = TokenId::KwdNamespace;

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

    nativeTypes["u64"]    = &g_TypeInfoU64;
    nativeTypes["u32"]    = &g_TypeInfoU32;
    nativeTypes["u16"]    = &g_TypeInfoU16;
    nativeTypes["u8"]     = &g_TypeInfoU8;
    nativeTypes["s64"]    = &g_TypeInfoS64;
    nativeTypes["s32"]    = &g_TypeInfoS32;
    nativeTypes["s16"]    = &g_TypeInfoS16;
    nativeTypes["s8"]     = &g_TypeInfoS8;
    nativeTypes["f32"]    = &g_TypeInfoF32;
    nativeTypes["f64"]    = &g_TypeInfoF64;
    nativeTypes["bool"]   = &g_TypeInfoBool;
    nativeTypes["char"]   = &g_TypeInfoChar;
    nativeTypes["string"] = &g_TypeInfoString;
}

string LanguageSpec::tokenToName(TokenId id)
{
    switch (id)
    {
    case TokenId::SymColon:
        return ":";
    case TokenId::SymSemiColon:
        return ";";
    case TokenId::SymLeftCurly:
        return "{";
    case TokenId::SymRightCurly:
        return "}";
    }

    return "???";
}