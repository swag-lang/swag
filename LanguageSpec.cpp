#include "pch.h"
#include "LanguageSpec.h"
#include "TypeManager.h"

LanguageSpec g_LangSpec;

void LanguageSpec::setup()
{
    keywords["#skip"]           = TokenId::CompilerSkip;
    keywords["#module"]         = TokenId::CompilerModule;
    keywords["#unittest"]       = TokenId::CompilerUnitTest;
    keywords["#assert"]         = TokenId::CompilerAssert;
    keywords["#print"]          = TokenId::CompilerPrint;
    keywords["#run"]            = TokenId::CompilerRun;
    keywords["#ast"]            = TokenId::CompilerAst;
    keywords["#file"]           = TokenId::CompilerFile;
    keywords["#line"]           = TokenId::CompilerLine;
    keywords["#cfg"]            = TokenId::CompilerBuildCfg;
    keywords["#target"]         = TokenId::CompilerTarget;
    keywords["#function"]       = TokenId::CompilerFunction;
    keywords["#callerfile"]     = TokenId::CompilerCallerFile;
    keywords["#callerline"]     = TokenId::CompilerCallerLine;
    keywords["#callerfunction"] = TokenId::CompilerCallerFunction;
    keywords["#swagversion"]    = TokenId::CompilerBuildVersion;
    keywords["#swagrevision"]   = TokenId::CompilerBuildRevision;
    keywords["#swagbuildnum"]   = TokenId::CompilerBuildNum;
    keywords["#import"]         = TokenId::CompilerImport;
    keywords["#test"]           = TokenId::CompilerFuncTest;
    keywords["#init"]           = TokenId::CompilerFuncInit;
    keywords["#drop"]           = TokenId::CompilerFuncDrop;
    keywords["#main"]           = TokenId::CompilerFuncMain;
    keywords["#if"]             = TokenId::CompilerIf;
    keywords["#else"]           = TokenId::CompilerElse;
    keywords["#elif"]           = TokenId::CompilerElseIf;
    keywords["#code"]           = TokenId::CompilerCode;
    keywords["#mixin"]          = TokenId::CompilerMixin;
    keywords["#inline"]         = TokenId::CompilerInline;
    keywords["#macro"]          = TokenId::CompilerMacro;
    keywords["#foreignlib"]     = TokenId::CompilerForeignLib;

    keywords["var"]         = TokenId::KwdVar;
    keywords["let"]         = TokenId::KwdLet;
    keywords["const"]       = TokenId::KwdConst;
    keywords["namespace"]   = TokenId::KwdNamespace;
    keywords["public"]      = TokenId::KwdPublic;
    keywords["private"]     = TokenId::KwdPrivate;
    keywords["internal"]    = TokenId::KwdInternal;
    keywords["readonly"]    = TokenId::KwdReadOnly;
    keywords["readwrite"]   = TokenId::KwdReadWrite;
    keywords["alias"]       = TokenId::KwdAlias;
    keywords["enum"]        = TokenId::KwdEnum;
    keywords["enumvalue"]   = TokenId::KwdEnumValue;
    keywords["impl"]        = TokenId::KwdImpl;
    keywords["struct"]      = TokenId::KwdStruct;
    keywords["interface"]   = TokenId::KwdInterface;
    keywords["union"]       = TokenId::KwdUnion;
    keywords["func"]        = TokenId::KwdFunc;
    keywords["true"]        = TokenId::KwdTrue;
    keywords["false"]       = TokenId::KwdFalse;
    keywords["null"]        = TokenId::KwdNull;
    keywords["attr"]        = TokenId::KwdAttr;
    keywords["return"]      = TokenId::KwdReturn;
    keywords["using"]       = TokenId::KwdUsing;
    keywords["cast"]        = TokenId::KwdCast;
    keywords["acast"]       = TokenId::KwdAutoCast;
    keywords["if"]          = TokenId::KwdIf;
    keywords["else"]        = TokenId::KwdElse;
    keywords["while"]       = TokenId::KwdWhile;
    keywords["break"]       = TokenId::KwdBreak;
    keywords["continue"]    = TokenId::KwdContinue;
    keywords["loop"]        = TokenId::KwdLoop;
    keywords["visit"]       = TokenId::KwdVisit;
    keywords["move"]        = TokenId::KwdMove;
    keywords["nodrop"]      = TokenId::KwdNoDrop;
    keywords["switch"]      = TokenId::KwdSwitch;
    keywords["case"]        = TokenId::KwdCase;
    keywords["fallthrough"] = TokenId::KwdFallThrough;
    keywords["default"]     = TokenId::KwdDefault;
    keywords["for"]         = TokenId::KwdFor;
    keywords["defer"]       = TokenId::KwdDefer;
    keywords["init"]        = TokenId::KwdInit;
    keywords["drop"]        = TokenId::KwdDrop;
    keywords["deref"]       = TokenId::KwdDeRef;
    keywords["code"]        = TokenId::KwdCode;
    keywords["label"]       = TokenId::KwdLabel;

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
    keywords["any"]    = TokenId::NativeType;
    keywords["void"]   = TokenId::NativeType;

    keywords["@sizeof"]      = TokenId::IntrinsicSizeOf;
    keywords["@typeof"]      = TokenId::IntrinsicTypeOf;
    keywords["@kindof"]      = TokenId::IntrinsicKindOf;
    keywords["@countof"]     = TokenId::IntrinsicCountOf;
    keywords["@dataof"]      = TokenId::IntrinsicDataOf;
    keywords["@mkslice"]     = TokenId::IntrinsicMakeSlice;
    keywords["@mkinterface"] = TokenId::IntrinsicMakeInterface;
    keywords["@index"]       = TokenId::IntrinsicIndex;
    keywords["@print"]       = TokenId::IntrinsicPrint;
    keywords["@assert"]      = TokenId::IntrinsicAssert;
    keywords["@alloc"]       = TokenId::IntrinsicAlloc;
    keywords["@realloc"]     = TokenId::IntrinsicRealloc;
    keywords["@free"]        = TokenId::IntrinsicFree;
    keywords["@memcpy"]      = TokenId::IntrinsicMemCpy;
    keywords["@memset"]      = TokenId::IntrinsicMemSet;
    keywords["@memcmp"]      = TokenId::IntrinsicMemCmp;
    keywords["@getcontext"]  = TokenId::IntrinsicGetContext;
    keywords["@setcontext"]  = TokenId::IntrinsicSetContext;
    keywords["@args"]        = TokenId::IntrinsicArguments;
    keywords["@isbytecode"]  = TokenId::IntrinsicIsByteCode;

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
    nativeTypes["any"]    = g_TypeMgr.typeInfoAny;
    nativeTypes["void"]   = g_TypeMgr.typeInfoVoid;
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
    case TokenId::SymMinusGreat:
        return "->";
    case TokenId::SymEqual:
        return "=";
    }

    return "???";
}