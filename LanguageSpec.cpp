#include "pch.h"
#include "LanguageSpec.h"
#include "TypeManager.h"

LanguageSpec g_LangSpec;

void LanguageSpec::setup()
{
    keywords["#public"]         = TokenId::CompilerPublic;
    keywords["#skip"]           = TokenId::CompilerSkip;
    keywords["#module"]         = TokenId::CompilerModule;
    keywords["#unittest"]       = TokenId::CompilerUnitTest;
    keywords["#assert"]         = TokenId::CompilerAssert;
    keywords["#print"]          = TokenId::CompilerPrint;
    keywords["#run"]            = TokenId::CompilerRun;
    keywords["#ast"]            = TokenId::CompilerAst;
    keywords["#cfg"]            = TokenId::CompilerBuildCfg;
    keywords["#arch"]           = TokenId::CompilerArch;
    keywords["#os"]             = TokenId::CompilerOs;
    keywords["#hastag"]         = TokenId::CompilerHasTag;
    keywords["#tagval"]         = TokenId::CompilerTagVal;
    keywords["#file"]           = TokenId::CompilerFile;
    keywords["#line"]           = TokenId::CompilerLine;
    keywords["#function"]       = TokenId::CompilerFunction;
    keywords["#callerfunction"] = TokenId::CompilerCallerFunction;
    keywords["#location"]       = TokenId::CompilerLocation;
    keywords["#callerlocation"] = TokenId::CompilerCallerLocation;
    keywords["#swagversion"]    = TokenId::CompilerBuildVersion;
    keywords["#swagrevision"]   = TokenId::CompilerBuildRevision;
    keywords["#swagbuildnum"]   = TokenId::CompilerBuildNum;
    keywords["#import"]         = TokenId::CompilerImport;
    keywords["#test"]           = TokenId::CompilerFuncTest;
    keywords["#init"]           = TokenId::CompilerFuncInit;
    keywords["#drop"]           = TokenId::CompilerFuncDrop;
    keywords["#main"]           = TokenId::CompilerFuncMain;
    keywords["#compiler"]       = TokenId::CompilerFuncCompiler;
    keywords["#if"]             = TokenId::CompilerIf;
    keywords["#else"]           = TokenId::CompilerElse;
    keywords["#elif"]           = TokenId::CompilerElseIf;
    keywords["#code"]           = TokenId::CompilerCode;
    keywords["#mixin"]          = TokenId::CompilerMixin;
    keywords["#inline"]         = TokenId::CompilerInline;
    keywords["#macro"]          = TokenId::CompilerMacro;
    keywords["#foreignlib"]     = TokenId::CompilerForeignLib;
    keywords["#bake"]           = TokenId::CompilerBake;

    keywords["var"]         = TokenId::KwdVar;
    keywords["let"]         = TokenId::KwdLet;
    keywords["const"]       = TokenId::KwdConst;
    keywords["namespace"]   = TokenId::KwdNamespace;
    keywords["public"]      = TokenId::KwdPublic;
    keywords["private"]     = TokenId::KwdPrivate;
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
    keywords["code"]        = TokenId::KwdCode;
    keywords["label"]       = TokenId::KwdLabel;

    keywords["u64"]      = TokenId::NativeType;
    keywords["u32"]      = TokenId::NativeType;
    keywords["u16"]      = TokenId::NativeType;
    keywords["u8"]       = TokenId::NativeType;
    keywords["s64"]      = TokenId::NativeType;
    keywords["s32"]      = TokenId::NativeType;
    keywords["s16"]      = TokenId::NativeType;
    keywords["s8"]       = TokenId::NativeType;
    keywords["f32"]      = TokenId::NativeType;
    keywords["f64"]      = TokenId::NativeType;
    keywords["bool"]     = TokenId::NativeType;
    keywords["char"]     = TokenId::NativeType;
    keywords["string"]   = TokenId::NativeType;
    keywords["any"]      = TokenId::NativeType;
    keywords["void"]     = TokenId::NativeType;
    keywords["typeinfo"] = TokenId::NativeType;

    keywords["@init"]        = TokenId::IntrinsicInit;
    keywords["@drop"]        = TokenId::IntrinsicDrop;
    keywords["@sizeof"]      = TokenId::IntrinsicSizeOf;
    keywords["@typeof"]      = TokenId::IntrinsicTypeOf;
    keywords["@kindof"]      = TokenId::IntrinsicKindOf;
    keywords["@countof"]     = TokenId::IntrinsicCountOf;
    keywords["@dataof"]      = TokenId::IntrinsicDataOf;
    keywords["@mkslice"]     = TokenId::IntrinsicMakeSlice;
    keywords["@mkany"]       = TokenId::IntrinsicMakeAny;
    keywords["@mkinterface"] = TokenId::IntrinsicMakeInterface;
    keywords["@index"]       = TokenId::IntrinsicIndex;
    keywords["@print"]       = TokenId::IntrinsicPrint;
    keywords["@assert"]      = TokenId::IntrinsicAssert;
    keywords["@error"]       = TokenId::IntrinsicError;
    keywords["@alloc"]       = TokenId::IntrinsicAlloc;
    keywords["@realloc"]     = TokenId::IntrinsicRealloc;
    keywords["@free"]        = TokenId::IntrinsicFree;
    keywords["@memcpy"]      = TokenId::IntrinsicMemCpy;
    keywords["@memset"]      = TokenId::IntrinsicMemSet;
    keywords["@memcmp"]      = TokenId::IntrinsicMemCmp;
    keywords["@strcmp"]      = TokenId::IntrinsicStrCmp;
    keywords["@typecmp"]     = TokenId::IntrinsicTypeCmp;
    keywords["@getcontext"]  = TokenId::IntrinsicGetContext;
    keywords["@setcontext"]  = TokenId::IntrinsicSetContext;
    keywords["@args"]        = TokenId::IntrinsicArguments;
    keywords["@isbytecode"]  = TokenId::IntrinsicIsByteCode;
    keywords["@compiler"]    = TokenId::IntrinsicCompiler;

    keywords["@sqrt"]  = TokenId::IntrinsicSqrt;
    keywords["@sin"]   = TokenId::IntrinsicSin;
    keywords["@cos"]   = TokenId::IntrinsicCos;
    keywords["@tan"]   = TokenId::IntrinsicTan;
    keywords["@sinh"]  = TokenId::IntrinsicSinh;
    keywords["@cosh"]  = TokenId::IntrinsicCosh;
    keywords["@tanh"]  = TokenId::IntrinsicTanh;
    keywords["@asin"]  = TokenId::IntrinsicASin;
    keywords["@acos"]  = TokenId::IntrinsicACos;
    keywords["@atan"]  = TokenId::IntrinsicATan;
    keywords["@log"]   = TokenId::IntrinsicLog;
    keywords["@log2"]  = TokenId::IntrinsicLog2;
    keywords["@log10"] = TokenId::IntrinsicLog10;
    keywords["@floor"] = TokenId::IntrinsicFloor;
    keywords["@ceil"]  = TokenId::IntrinsicCeil;
    keywords["@trunc"] = TokenId::IntrinsicTrunc;
    keywords["@round"] = TokenId::IntrinsicRound;
    keywords["@abs"]   = TokenId::IntrinsicAbs;
    keywords["@exp"]   = TokenId::IntrinsicExp;
    keywords["@exp2"]  = TokenId::IntrinsicExp2;
    keywords["@pow"]   = TokenId::IntrinsicPow;

    nativeTypes["u64"]      = LiteralType::TT_U64;
    nativeTypes["u32"]      = LiteralType::TT_U32;
    nativeTypes["u16"]      = LiteralType::TT_U16;
    nativeTypes["u8"]       = LiteralType::TT_U8;
    nativeTypes["s64"]      = LiteralType::TT_S64;
    nativeTypes["s32"]      = LiteralType::TT_S32;
    nativeTypes["s16"]      = LiteralType::TT_S16;
    nativeTypes["s8"]       = LiteralType::TT_S8;
    nativeTypes["f32"]      = LiteralType::TT_F32;
    nativeTypes["f64"]      = LiteralType::TT_F64;
    nativeTypes["bool"]     = LiteralType::TT_BOOL;
    nativeTypes["char"]     = LiteralType::TT_CHAR;
    nativeTypes["string"]   = LiteralType::TT_STRING;
    nativeTypes["any"]      = LiteralType::TT_ANY;
    nativeTypes["void"]     = LiteralType::TT_VOID;
    nativeTypes["typeinfo"] = LiteralType::TT_TYPE;
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