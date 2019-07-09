#include "pch.h"
#include "LanguageSpec.h"
#include "Global.h"
#include "TypeManager.h"

LanguageSpec g_LangSpec;

void LanguageSpec::setup()
{
    keywords["#module"]   = TokenId::CompilerModule;
    keywords["#unittest"] = TokenId::CompilerUnitTest;
    keywords["#assert"]   = TokenId::CompilerAssert;
    keywords["#print"]    = TokenId::CompilerPrint;
    keywords["#run"]      = TokenId::CompilerRun;
    keywords["#file"]     = TokenId::CompilerFile;
    keywords["#line"]     = TokenId::CompilerLine;
    keywords["#import"]   = TokenId::CompilerImport;
    keywords["#version"]  = TokenId::CompilerVersion;
    keywords["#else"]     = TokenId::CompilerElse;
    keywords["#test"]     = TokenId::CompilerTest;

    keywords["@print"]  = TokenId::IntrisicPrint;
    keywords["@assert"] = TokenId::IntrisicAssert;
    keywords["@prop"]   = TokenId::IntrisicProp;

    keywords["var"]       = TokenId::KwdVar;
    keywords["const"]     = TokenId::KwdConst;
    keywords["namespace"] = TokenId::KwdNamespace;
    keywords["type"]      = TokenId::KwdType;
    keywords["enum"]      = TokenId::KwdEnum;
    keywords["func"]      = TokenId::KwdFunc;
    keywords["true"]      = TokenId::KwdTrue;
    keywords["false"]     = TokenId::KwdFalse;
    keywords["null"]      = TokenId::KwdNull;
    keywords["attr"]      = TokenId::KwdAttr;
    keywords["return"]    = TokenId::KwdReturn;
    keywords["using"]     = TokenId::KwdUsing;
    keywords["cast"]      = TokenId::KwdCast;
    keywords["if"]        = TokenId::KwdIf;
    keywords["else"]      = TokenId::KwdElse;
    keywords["while"]     = TokenId::KwdWhile;
    keywords["break"]     = TokenId::KwdBreak;
    keywords["continue"]  = TokenId::KwdContinue;
    keywords["loop"]      = TokenId::KwdLoop;
    keywords["index"]     = TokenId::KwdIndex;

    keywords["u64"]    = TokenId::NativeType;
    keywords["u32"]    = TokenId::NativeType;
    keywords["u16"]    = TokenId::NativeType;
    keywords["u8"]     = TokenId::NativeType;
    keywords["s64"]    = TokenId::NativeType;
    keywords["s32"]    = TokenId::NativeType;
    keywords["s16"]    = TokenId::NativeType;
    keywords["s8"]     = TokenId::NativeType;
    keywords["f32"]    = TokenId::NativeType;
    keywords["f64"]    = TokenId::NativeType;
    keywords["bool"]   = TokenId::NativeType;
    keywords["char"]   = TokenId::NativeType;
    keywords["string"] = TokenId::NativeType;

    nativeTypes["u64"]    = g_TypeMgr.typeInfoU64;
    nativeTypes["u32"]    = g_TypeMgr.typeInfoU32;
    nativeTypes["u16"]    = g_TypeMgr.typeInfoU16;
    nativeTypes["u8"]     = g_TypeMgr.typeInfoU8;
    nativeTypes["s64"]    = g_TypeMgr.typeInfoS64;
    nativeTypes["s32"]    = g_TypeMgr.typeInfoS32;
    nativeTypes["s16"]    = g_TypeMgr.typeInfoS16;
    nativeTypes["s8"]     = g_TypeMgr.typeInfoS8;
    nativeTypes["f32"]    = g_TypeMgr.typeInfoF32;
    nativeTypes["f64"]    = g_TypeMgr.typeInfoF64;
    nativeTypes["bool"]   = g_TypeMgr.typeInfoBool;
    nativeTypes["char"]   = g_TypeMgr.typeInfoChar;
    nativeTypes["string"] = g_TypeMgr.typeInfoString;

    properties["sizeof"] = Property::SizeOf;
    properties["count"]  = Property::Count;
    properties["data"]   = Property::Data;
}

string LanguageSpec::tokenToName(TokenId id)
{
    switch (id)
    {
    case TokenId::SymComma:
        return ",";
    case TokenId::SymColon:
        return ":";
    case TokenId::SymSemiColon:
        return ";";
    case TokenId::SymLeftCurly:
        return "{";
    case TokenId::SymRightCurly:
        return "}";
    case TokenId::SymLeftParen:
        return "(";
    case TokenId::SymRightParen:
        return ")";
    case TokenId::SymLeftSquare:
        return "[";
    case TokenId::SymRightSquare:
        return "]";
    case TokenId::SymDot:
        return ".";
    case TokenId::SymExclam:
        return "!";
    }

    return "???";
}