#include "pch.h"
#include "LanguageSpec.h"
#include "TypeManager.h"

LanguageSpec g_LangSpec;

void LanguageSpec::setupNames()
{
    name__setupRuntime   = "__setupRuntime";
    name__panic          = "__panic";
    name__clearerr       = "__clearerr";
    name__pusherr        = "__pusherr";
    name__poperr         = "__poperr";
    name__geterr         = "__geterr";
    name__tlsAlloc       = "__tlsAlloc";
    name__tlsGetValue    = "__tlsGetValue";
    name__tlsSetValue    = "__tlsSetValue";
    name__tlsGetPtr      = "__tlsGetPtr";
    name__initStackTrace = "__initStackTrace";
    name__stackTrace     = "__stackTrace";
    name__loaddll        = "__loaddll";
    name__exit           = "__exit";

    name_memcpy  = "memcpy";
    name_memset  = "memset";
    name_memmove = "memmove";
    name_memcmp  = "memcmp";
    name_strlen  = "strlen";
    name_malloc  = "malloc";
    name_realloc = "realloc";
    name_free    = "free";

    name_swag = "swag";
    name_Swag = "Swag";
    name_self = "self";
    name_Self = "Self";

    name_opInitGenerated     = "opInitGenerated";
    name_opDropGenerated     = "opDropGenerated";
    name_opRelocGenerated    = "opRelocGenerated";
    name_opPostCopyGenerated = "opPostCopyGenerated";
    name_opPostMoveGenerated = "opPostMoveGenerated";

    name_Safety     = "Safety";
    name_Optim      = "Optim";
    name_SelectIf   = "SelectIf";
    name_Pack       = "Pack";
    name_Align      = "Align";
    name_Complete   = "Complete";
    name_AttrUsage  = "AttrUsage";
    name_AttrMulti  = "AttrMulti";
    name_Global     = "Global";
    name_Strict     = "Strict";
    name_PrintBc    = "PrintBc";
    name_ExportType = "ExportType";

    name_Swag_AttrUsage  = "Swag.AttrUsage";
    name_Swag_AttrMulti  = "Swag.AttrMulti";
    name_Swag_Align      = "Swag.Align";
    name_Swag_Safety     = "Swag.Safety";
    name_Swag_Optim      = "Swag.Optim";
    name_Swag_SelectIf   = "Swag.SelectIf";
    name_Swag_Pack       = "Swag.Pack";
    name_Swag_Foreign    = "Swag.Foreign";
    name_Swag_Deprecated = "Swag.Deprecated";
    name_Swag_Opaque     = "Swag.Opaque";
    name_Swag_Offset     = "Swag.Offset";
    name_Swag_ExportType = "Swag.ExportType";

    name_opBinary      = "opBinary";
    name_opUnary       = "opUnary";
    name_opAssign      = "opAssign";
    name_opIndexAssign = "opIndexAssign";
    name_opCast        = "opCast";
    name_opEquals      = "opEquals";
    name_opCmp         = "opCmp";
    name_opPostCopy    = "opPostCopy";
    name_opPostMove    = "opPostMove";
    name_opDrop        = "opDrop";
    name_opCount       = "opCount";
    name_opData        = "opData";
    name_opAffect      = "opAffect";
    name_opSlice       = "opSlice";
    name_opIndex       = "opIndex";
    name_opIndexAffect = "opIndexAffect";
    name_opInit        = "opInit";

    name_TypeInfo          = "TypeInfo";
    name_TypeInfoNative    = "TypeInfoNative";
    name_TypeInfoPointer   = "TypeInfoPointer";
    name_TypeInfoReference = "TypeInfoReference";
    name_TypeInfoStruct    = "TypeInfoStruct";
    name_TypeInfoParam     = "TypeInfoParam";
    name_TypeInfoFunc      = "TypeInfoFunc";
    name_TypeInfoEnum      = "TypeInfoEnum";
    name_TypeInfoVariadic  = "TypeInfoVariadic";
    name_TypeInfoArray     = "TypeInfoArray";
    name_TypeInfoSlice     = "TypeInfoSlice";
    name_TypeInfoGeneric   = "TypeInfoGeneric";
    name_TypeInfoAlias     = "TypeInfoAlias";

    name_SystemAllocator        = "SystemAllocator";
    name_CompilerSourceLocation = "CompilerSourceLocation";
    name_BuildCfg               = "BuildCfg";
    name_AttributeUsage         = "AttributeUsage";
    name_Swag_CompilerMsgMask   = "Swag.CompilerMsgMask";

    name_export       = "export";
    name_generated    = "generated";
    name_foreignlib   = "foreignlib";
    name_skip         = "skip";
    name_testpass     = "testpass";
    name_lexer        = "lexer";
    name_syntax       = "syntax";
    name_semantic     = "semantic";
    name_backend      = "backend";
    name_testerror    = "testerror";
    name_testerrors   = "testerrors";
    name_testwarning  = "testwarning";
    name_testwarnings = "testwarnings";
    name_location     = "location";
    name_version      = "version";

    name_safe    = "safe";
    name_small   = "small";
    name_nodrop  = "nodrop";
    name_move    = "move";
    name_moveraw = "moveraw";

    name_name       = "name";
    name_msg        = "msg";
    name_module     = "module";
    name_function   = "function";
    name_nullptr    = "nullptr";
    name_boundcheck = "boundcheck";
    name_overflow   = "overflow";
    name_math       = "math";
    name_castany    = "castany";
    name_bytecode   = "bytecode";
    name_usage      = "usage";
    name_what       = "what";
    name_value      = "value";
    name_disk       = "disk";
    name_retval     = "retval";
    name_methods    = "methods";    

    name_atalias       = "@alias";
    name_atdrop        = "@drop";
    name_atpostCopy    = "@postCopy";
    name_atpostMove    = "@postMove";
    name_atargs        = "@args";
    name_atseterr      = "@seterr";
    name_atpanic       = "@panic";
    name_aterrormsg    = "@errormsg";
    name_atinterfaceof = "@interfaceof";
    name_atstrcmp      = "@strcmp";
    name_attypecmp     = "@typecmp";

    name_powf   = "powf";
    name_atan2f = "atan2f";
    name_pow    = "pow";
    name_atan2  = "atan2";
    name_sinf   = "sinf";
    name_cosf   = "cosf";
    name_tanf   = "tanf";
    name_sinhf  = "sinhf";
    name_coshf  = "coshf";
    name_tanhf  = "tanhf";
    name_asinf  = "asinf";
    name_acosf  = "acosf";
    name_atanf  = "atanf";
    name_logf   = "logf";
    name_log2f  = "log2f";
    name_log10f = "log10f";
    name_floorf = "floorf";
    name_ceilf  = "ceilf";
    name_truncf = "truncf";
    name_roundf = "roundf";
    name_expf   = "expf";
    name_exp2f  = "exp2f";
    name_sin    = "sin";
    name_cos    = "cos";
    name_tan    = "tan";
    name_sinh   = "sinh";
    name_cosh   = "cosh";
    name_tanh   = "tanh";
    name_asin   = "asin";
    name_acos   = "acos";
    name_atan   = "atan";
    name_log    = "log";
    name_log2   = "log2";
    name_log10  = "log10";
    name_floor  = "floor";
    name_ceil   = "ceil";
    name_trunc  = "trunc";
    name_round  = "round";
    name_exp    = "exp";
    name_exp2   = "exp2";
}

void LanguageSpec::setupKeywords()
{
    keywords.add("#global", {TokenId::CompilerGlobal, LiteralType::TT_MAX});
    keywords.add("#assert", {TokenId::CompilerAssert, LiteralType::TT_MAX});
    keywords.add("#semerror", {TokenId::CompilerSemError, LiteralType::TT_MAX});
    keywords.add("#print", {TokenId::CompilerPrint, LiteralType::TT_MAX});
    keywords.add("#run", {TokenId::CompilerRun, LiteralType::TT_MAX});
    keywords.add("#ast", {TokenId::CompilerAst, LiteralType::TT_MAX});
    keywords.add("#selectif", {TokenId::CompilerSelectIf, LiteralType::TT_MAX});
    keywords.add("#checkif", {TokenId::CompilerCheckIf, LiteralType::TT_MAX});
    keywords.add("#cfg", {TokenId::CompilerBuildCfg, LiteralType::TT_MAX});
    keywords.add("#arch", {TokenId::CompilerArch, LiteralType::TT_MAX});
    keywords.add("#os", {TokenId::CompilerOs, LiteralType::TT_MAX});
    keywords.add("#abi", {TokenId::CompilerAbi, LiteralType::TT_MAX});
    keywords.add("#hastag", {TokenId::CompilerHasTag, LiteralType::TT_MAX});
    keywords.add("#gettag", {TokenId::CompilerGetTag, LiteralType::TT_MAX});
    keywords.add("#file", {TokenId::CompilerFile, LiteralType::TT_MAX});
    keywords.add("#line", {TokenId::CompilerLine, LiteralType::TT_MAX});
    keywords.add("#function", {TokenId::CompilerFunction, LiteralType::TT_MAX});
    keywords.add("#callerfunction", {TokenId::CompilerCallerFunction, LiteralType::TT_MAX});
    keywords.add("#location", {TokenId::CompilerLocation, LiteralType::TT_MAX});
    keywords.add("#callerlocation", {TokenId::CompilerCallerLocation, LiteralType::TT_MAX});
    keywords.add("#swagversion", {TokenId::CompilerBuildVersion, LiteralType::TT_MAX});
    keywords.add("#swagrevision", {TokenId::CompilerBuildRevision, LiteralType::TT_MAX});
    keywords.add("#swagbuildnum", {TokenId::CompilerBuildNum, LiteralType::TT_MAX});
    keywords.add("#swagos", {TokenId::CompilerSwagOs, LiteralType::TT_MAX});
    keywords.add("#dependencies", {TokenId::CompilerDependencies, LiteralType::TT_MAX});
    keywords.add("#import", {TokenId::CompilerImport, LiteralType::TT_MAX});
    keywords.add("#test", {TokenId::CompilerFuncTest, LiteralType::TT_MAX});
    keywords.add("#init", {TokenId::CompilerFuncInit, LiteralType::TT_MAX});
    keywords.add("#drop", {TokenId::CompilerFuncDrop, LiteralType::TT_MAX});
    keywords.add("#main", {TokenId::CompilerFuncMain, LiteralType::TT_MAX});
    keywords.add("#compiler", {TokenId::CompilerFuncCompiler, LiteralType::TT_MAX});
    keywords.add("#placeholder", {TokenId::CompilerPlaceHolder, LiteralType::TT_MAX});
    keywords.add("#if", {TokenId::CompilerIf, LiteralType::TT_MAX});
    keywords.add("#else", {TokenId::CompilerElse, LiteralType::TT_MAX});
    keywords.add("#elif", {TokenId::CompilerElseIf, LiteralType::TT_MAX});
    keywords.add("#code", {TokenId::CompilerCode, LiteralType::TT_MAX});
    keywords.add("#mixin", {TokenId::CompilerMixin, LiteralType::TT_MAX});
    keywords.add("#inline", {TokenId::CompilerInline, LiteralType::TT_MAX});
    keywords.add("#macro", {TokenId::CompilerMacro, LiteralType::TT_MAX});
    keywords.add("#defined", {TokenId::CompilerDefined, LiteralType::TT_MAX});
    keywords.add("#load", {TokenId::CompilerLoad, LiteralType::TT_MAX});
    keywords.add("#scopefct", {TokenId::CompilerScopeFct, LiteralType::TT_MAX});

    keywords.add("var", {TokenId::KwdVar, LiteralType::TT_MAX});
    keywords.add("const", {TokenId::KwdConst, LiteralType::TT_MAX});
    keywords.add("namespace", {TokenId::KwdNamespace, LiteralType::TT_MAX});
    keywords.add("public", {TokenId::KwdPublic, LiteralType::TT_MAX});
    keywords.add("protected", {TokenId::KwdProtected, LiteralType::TT_MAX});
    keywords.add("private", {TokenId::KwdPrivate, LiteralType::TT_MAX});
    keywords.add("alias", {TokenId::KwdAlias, LiteralType::TT_MAX});
    keywords.add("enum", {TokenId::KwdEnum, LiteralType::TT_MAX});
    keywords.add("impl", {TokenId::KwdImpl, LiteralType::TT_MAX});
    keywords.add("struct", {TokenId::KwdStruct, LiteralType::TT_MAX});
    keywords.add("union", {TokenId::KwdUnion, LiteralType::TT_MAX});
    keywords.add("interface", {TokenId::KwdInterface, LiteralType::TT_MAX});
    keywords.add("func", {TokenId::KwdFunc, LiteralType::TT_MAX});
    keywords.add("method", {TokenId::KwdMethod, LiteralType::TT_MAX});
    keywords.add("true", {TokenId::KwdTrue, LiteralType::TT_MAX});
    keywords.add("false", {TokenId::KwdFalse, LiteralType::TT_MAX});
    keywords.add("null", {TokenId::KwdNull, LiteralType::TT_MAX});
    keywords.add("attr", {TokenId::KwdAttr, LiteralType::TT_MAX});
    keywords.add("return", {TokenId::KwdReturn, LiteralType::TT_MAX});
    keywords.add("retval", {TokenId::KwdRetVal, LiteralType::TT_MAX});
    keywords.add("using", {TokenId::KwdUsing, LiteralType::TT_MAX});
    keywords.add("cast", {TokenId::KwdCast, LiteralType::TT_MAX});
    keywords.add("bitcast", {TokenId::KwdBitCast, LiteralType::TT_MAX});
    keywords.add("acast", {TokenId::KwdAutoCast, LiteralType::TT_MAX});
    keywords.add("if", {TokenId::KwdIf, LiteralType::TT_MAX});
    keywords.add("else", {TokenId::KwdElse, LiteralType::TT_MAX});
    keywords.add("while", {TokenId::KwdWhile, LiteralType::TT_MAX});
    keywords.add("break", {TokenId::KwdBreak, LiteralType::TT_MAX});
    keywords.add("continue", {TokenId::KwdContinue, LiteralType::TT_MAX});
    keywords.add("loop", {TokenId::KwdLoop, LiteralType::TT_MAX});
    keywords.add("visit", {TokenId::KwdVisit, LiteralType::TT_MAX});
    keywords.add("switch", {TokenId::KwdSwitch, LiteralType::TT_MAX});
    keywords.add("case", {TokenId::KwdCase, LiteralType::TT_MAX});
    keywords.add("fallthrough", {TokenId::KwdFallThrough, LiteralType::TT_MAX});
    keywords.add("default", {TokenId::KwdDefault, LiteralType::TT_MAX});
    keywords.add("for", {TokenId::KwdFor, LiteralType::TT_MAX});
    keywords.add("defer", {TokenId::KwdDefer, LiteralType::TT_MAX});
    keywords.add("errdefer", {TokenId::KwdErrDefer, LiteralType::TT_MAX});
    keywords.add("code", {TokenId::KwdCode, LiteralType::TT_MAX});
    keywords.add("label", {TokenId::KwdLabel, LiteralType::TT_MAX});
    keywords.add("try", {TokenId::KwdTry, LiteralType::TT_MAX});
    keywords.add("catch", {TokenId::KwdCatch, LiteralType::TT_MAX});
    keywords.add("assume", {TokenId::KwdAssume, LiteralType::TT_MAX});
    keywords.add("throw", {TokenId::KwdThrow, LiteralType::TT_MAX});
    keywords.add("discard", {TokenId::KwdDiscard, LiteralType::TT_MAX});
    keywords.add("and", {TokenId::KwdAnd, LiteralType::TT_MAX});
    keywords.add("or", {TokenId::KwdOr, LiteralType::TT_MAX});
    keywords.add("orelse", {TokenId::KwdOrElse, LiteralType::TT_MAX});
    keywords.add("dref", {TokenId::KwdDeRef, LiteralType::TT_MAX});

    keywords.add("@spread", {TokenId::IntrinsicSpread, LiteralType::TT_MAX});
    keywords.add("@init", {TokenId::IntrinsicInit, LiteralType::TT_MAX});
    keywords.add("@drop", {TokenId::IntrinsicDrop, LiteralType::TT_MAX});
    keywords.add("@postmove", {TokenId::IntrinsicPostMove, LiteralType::TT_MAX});
    keywords.add("@postcopy", {TokenId::IntrinsicPostCopy, LiteralType::TT_MAX});
    keywords.add("@sizeof", {TokenId::IntrinsicSizeOf, LiteralType::TT_MAX});
    keywords.add("@alignof", {TokenId::IntrinsicAlignOf, LiteralType::TT_MAX});
    keywords.add("@offsetof", {TokenId::IntrinsicOffsetOf, LiteralType::TT_MAX});
    keywords.add("@typeof", {TokenId::IntrinsicTypeOf, LiteralType::TT_MAX});
    keywords.add("@kindof", {TokenId::IntrinsicKindOf, LiteralType::TT_MAX});
    keywords.add("@countof", {TokenId::IntrinsicCountOf, LiteralType::TT_MAX});
    keywords.add("@stringof", {TokenId::IntrinsicStringOf, LiteralType::TT_MAX});
    keywords.add("@dataof", {TokenId::IntrinsicDataOf, LiteralType::TT_MAX});
    keywords.add("@mkslice", {TokenId::IntrinsicMakeSlice, LiteralType::TT_MAX});
    keywords.add("@mkstring", {TokenId::IntrinsicMakeString, LiteralType::TT_MAX});
    keywords.add("@mkany", {TokenId::IntrinsicMakeAny, LiteralType::TT_MAX});
    keywords.add("@mkinterface", {TokenId::IntrinsicMakeInterface, LiteralType::TT_MAX});
    keywords.add("@mkcallback", {TokenId::IntrinsicMakeCallback, LiteralType::TT_MAX});
    keywords.add("@mkforeign", {TokenId::IntrinsicMakeForeign, LiteralType::TT_MAX});
    keywords.add("@index", {TokenId::IntrinsicIndex, LiteralType::TT_MAX});
    keywords.add("@isconstexpr", {TokenId::IntrinsicIsConstExpr, LiteralType::TT_MAX});
    keywords.add("@err", {TokenId::IntrinsicGetErr, LiteralType::TT_MAX});
    keywords.add("@seterr", {TokenId::IntrinsicSetErr, LiteralType::TT_MAX});
    keywords.add("@print", {TokenId::IntrinsicPrint, LiteralType::TT_MAX});
    keywords.add("@assert", {TokenId::IntrinsicAssert, LiteralType::TT_MAX});
    keywords.add("@bcdbg", {TokenId::IntrinsicBcDbg, LiteralType::TT_MAX});
    keywords.add("@panic", {TokenId::IntrinsicPanic, LiteralType::TT_MAX});
    keywords.add("@errormsg", {TokenId::IntrinsicErrorMsg, LiteralType::TT_MAX});
    keywords.add("@alloc", {TokenId::IntrinsicAlloc, LiteralType::TT_MAX});
    keywords.add("@realloc", {TokenId::IntrinsicRealloc, LiteralType::TT_MAX});
    keywords.add("@free", {TokenId::IntrinsicFree, LiteralType::TT_MAX});
    keywords.add("@memcpy", {TokenId::IntrinsicMemCpy, LiteralType::TT_MAX});
    keywords.add("@memmove", {TokenId::IntrinsicMemMove, LiteralType::TT_MAX});
    keywords.add("@memset", {TokenId::IntrinsicMemSet, LiteralType::TT_MAX});
    keywords.add("@memcmp", {TokenId::IntrinsicMemCmp, LiteralType::TT_MAX});
    keywords.add("@cstrlen", {TokenId::IntrinsicCStrLen, LiteralType::TT_MAX});
    keywords.add("@strcmp", {TokenId::IntrinsicStrCmp, LiteralType::TT_MAX});
    keywords.add("@typecmp", {TokenId::IntrinsicTypeCmp, LiteralType::TT_MAX});
    keywords.add("@getcontext", {TokenId::IntrinsicGetContext, LiteralType::TT_MAX});
    keywords.add("@setcontext", {TokenId::IntrinsicSetContext, LiteralType::TT_MAX});
    keywords.add("@args", {TokenId::IntrinsicArguments, LiteralType::TT_MAX});
    keywords.add("@isbytecode", {TokenId::IntrinsicIsByteCode, LiteralType::TT_MAX});
    keywords.add("@compiler", {TokenId::IntrinsicCompiler, LiteralType::TT_MAX});
    keywords.add("@atomadd", {TokenId::IntrinsicAtomicAdd, LiteralType::TT_MAX});
    keywords.add("@atomand", {TokenId::IntrinsicAtomicAnd, LiteralType::TT_MAX});
    keywords.add("@atomor", {TokenId::IntrinsicAtomicOr, LiteralType::TT_MAX});
    keywords.add("@atomxor", {TokenId::IntrinsicAtomicXor, LiteralType::TT_MAX});
    keywords.add("@atomxchg", {TokenId::IntrinsicAtomicXchg, LiteralType::TT_MAX});
    keywords.add("@atomcmpxchg", {TokenId::IntrinsicAtomicCmpXchg, LiteralType::TT_MAX});

    keywords.add("@sqrt", {TokenId::IntrinsicSqrt, LiteralType::TT_MAX});
    keywords.add("@sin", {TokenId::IntrinsicSin, LiteralType::TT_MAX});
    keywords.add("@cos", {TokenId::IntrinsicCos, LiteralType::TT_MAX});
    keywords.add("@tan", {TokenId::IntrinsicTan, LiteralType::TT_MAX});
    keywords.add("@sinh", {TokenId::IntrinsicSinh, LiteralType::TT_MAX});
    keywords.add("@cosh", {TokenId::IntrinsicCosh, LiteralType::TT_MAX});
    keywords.add("@tanh", {TokenId::IntrinsicTanh, LiteralType::TT_MAX});
    keywords.add("@asin", {TokenId::IntrinsicASin, LiteralType::TT_MAX});
    keywords.add("@acos", {TokenId::IntrinsicACos, LiteralType::TT_MAX});
    keywords.add("@atan", {TokenId::IntrinsicATan, LiteralType::TT_MAX});
    keywords.add("@atan2", {TokenId::IntrinsicATan2, LiteralType::TT_MAX});
    keywords.add("@log", {TokenId::IntrinsicLog, LiteralType::TT_MAX});
    keywords.add("@log2", {TokenId::IntrinsicLog2, LiteralType::TT_MAX});
    keywords.add("@log10", {TokenId::IntrinsicLog10, LiteralType::TT_MAX});
    keywords.add("@floor", {TokenId::IntrinsicFloor, LiteralType::TT_MAX});
    keywords.add("@ceil", {TokenId::IntrinsicCeil, LiteralType::TT_MAX});
    keywords.add("@trunc", {TokenId::IntrinsicTrunc, LiteralType::TT_MAX});
    keywords.add("@round", {TokenId::IntrinsicRound, LiteralType::TT_MAX});
    keywords.add("@abs", {TokenId::IntrinsicAbs, LiteralType::TT_MAX});
    keywords.add("@exp", {TokenId::IntrinsicExp, LiteralType::TT_MAX});
    keywords.add("@exp2", {TokenId::IntrinsicExp2, LiteralType::TT_MAX});
    keywords.add("@pow", {TokenId::IntrinsicPow, LiteralType::TT_MAX});
    keywords.add("@min", {TokenId::IntrinsicMin, LiteralType::TT_MAX});
    keywords.add("@max", {TokenId::IntrinsicMax, LiteralType::TT_MAX});
    keywords.add("@bitcountnz", {TokenId::IntrinsicBitCountNz, LiteralType::TT_MAX});
    keywords.add("@bitcounttz", {TokenId::IntrinsicBitCountTz, LiteralType::TT_MAX});
    keywords.add("@bitcountlz", {TokenId::IntrinsicBitCountLz, LiteralType::TT_MAX});
    keywords.add("@byteswap", {TokenId::IntrinsicByteSwap, LiteralType::TT_MAX});

    keywords.add("uint", {TokenId::NativeType, LiteralType::TT_UINT});
    keywords.add("int", {TokenId::NativeType, LiteralType::TT_INT});
    keywords.add("u64", {TokenId::NativeType, LiteralType::TT_U64});
    keywords.add("u32", {TokenId::NativeType, LiteralType::TT_U32});
    keywords.add("u16", {TokenId::NativeType, LiteralType::TT_U16});
    keywords.add("u8", {TokenId::NativeType, LiteralType::TT_U8});
    keywords.add("s64", {TokenId::NativeType, LiteralType::TT_S64});
    keywords.add("s32", {TokenId::NativeType, LiteralType::TT_S32});
    keywords.add("s16", {TokenId::NativeType, LiteralType::TT_S16});
    keywords.add("s8", {TokenId::NativeType, LiteralType::TT_S8});
    keywords.add("f32", {TokenId::NativeType, LiteralType::TT_F32});
    keywords.add("f64", {TokenId::NativeType, LiteralType::TT_F64});
    keywords.add("bool", {TokenId::NativeType, LiteralType::TT_BOOL});
    keywords.add("rune", {TokenId::NativeType, LiteralType::TT_RUNE});
    keywords.add("string", {TokenId::NativeType, LiteralType::TT_STRING});
    keywords.add("any", {TokenId::NativeType, LiteralType::TT_ANY});
    keywords.add("void", {TokenId::NativeType, LiteralType::TT_VOID});
    keywords.add("typeinfo", {TokenId::NativeType, LiteralType::TT_TYPE});
}

void LanguageSpec::setupAttributesFlags()
{
    attributesFlags.add("ConstExpr", ATTRIBUTE_CONSTEXPR);
    attributesFlags.add("PrintBc", ATTRIBUTE_PRINT_BC);
    attributesFlags.add("Test", ATTRIBUTE_TEST_FUNC);
    attributesFlags.add("Compiler", ATTRIBUTE_COMPILER);
    attributesFlags.add("EnumFlags", ATTRIBUTE_ENUM_FLAGS);
    attributesFlags.add("EnumIndex", ATTRIBUTE_ENUM_INDEX);
    attributesFlags.add("Foreign", ATTRIBUTE_FOREIGN);
    attributesFlags.add("Inline", ATTRIBUTE_INLINE);
    attributesFlags.add("Macro", ATTRIBUTE_MACRO);
    attributesFlags.add("Mixin", ATTRIBUTE_MIXIN);
    attributesFlags.add("Complete", ATTRIBUTE_COMPLETE);
    attributesFlags.add("Implicit", ATTRIBUTE_IMPLICIT);
    attributesFlags.add("NoReturn", ATTRIBUTE_NO_RETURN);
    attributesFlags.add("Discardable", ATTRIBUTE_DISCARDABLE);
    attributesFlags.add("Deprecated", ATTRIBUTE_DEPRECATED);
    attributesFlags.add("Global", ATTRIBUTE_GLOBAL);
    attributesFlags.add("Tls", ATTRIBUTE_TLS);
    attributesFlags.add("Strict", ATTRIBUTE_STRICT);
    attributesFlags.add("Callback", ATTRIBUTE_CALLBACK);
    attributesFlags.add("NoCopy", ATTRIBUTE_NO_COPY);
    attributesFlags.add("Opaque", ATTRIBUTE_OPAQUE);
}

void LanguageSpec::setupNativeTypes()
{
    nativeTypes.add("uint", LiteralType::TT_UINT);
    nativeTypes.add("int", LiteralType::TT_INT);
    nativeTypes.add("u64", LiteralType::TT_U64);
    nativeTypes.add("u32", LiteralType::TT_U32);
    nativeTypes.add("u16", LiteralType::TT_U16);
    nativeTypes.add("u8", LiteralType::TT_U8);
    nativeTypes.add("s64", LiteralType::TT_S64);
    nativeTypes.add("s32", LiteralType::TT_S32);
    nativeTypes.add("s16", LiteralType::TT_S16);
    nativeTypes.add("s8", LiteralType::TT_S8);
    nativeTypes.add("f32", LiteralType::TT_F32);
    nativeTypes.add("f64", LiteralType::TT_F64);
    nativeTypes.add("bool", LiteralType::TT_BOOL);
    nativeTypes.add("rune", LiteralType::TT_RUNE);
    nativeTypes.add("string", LiteralType::TT_STRING);
    nativeTypes.add("any", LiteralType::TT_ANY);
    nativeTypes.add("void", LiteralType::TT_VOID);
    nativeTypes.add("typeinfo", LiteralType::TT_TYPE);
}

void LanguageSpec::setup()
{
    setupNames();
    setupKeywords();
    setupAttributesFlags();
    setupNativeTypes();
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