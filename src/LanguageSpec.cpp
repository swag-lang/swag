#include "pch.h"
#include "LanguageSpec.h"
#include "TypeManager.h"

LanguageSpec* g_LangSpec = nullptr;

void LanguageSpec::setupNames()
{
    name_globalInit    = "__%s_globalInit";
    name_globalDrop    = "__%s_globalDrop";
    name_globalPreMain = "__%s_globalPreMain";
    name_getTypeTable  = "__%s_getTypeTable";

    name__setupRuntime        = "__setupRuntime";
    name__closeRuntime        = "__closeRuntime";
    name__dropGlobalVariables = "__dropGlobalVariables";
    name__compilererror       = "__compilererror";
    name__panic               = "__panic";
    name__pusherr             = "__pusherr";
    name__poperr              = "__poperr";
    name__seterr              = "__seterr";
    name__geterr              = "__geterr";
    name__tlsAlloc            = "__tlsAlloc";
    name__tlsGetValue         = "__tlsGetValue";
    name__tlsSetValue         = "__tlsSetValue";
    name__tlsGetPtr           = "__tlsGetPtr";
    name__initStackTrace      = "__initStackTrace";
    name__stackTrace          = "__stackTrace";
    name__loaddll             = "__loaddll";
    name__checkAny            = "__checkAny";

    name_swag = "swag";
    name_Swag = "Swag";
    name_self = "self";
    name_Self = "Self";

    name_opInitGenerated     = "opInitGenerated";
    name_opDropGenerated     = "opDropGenerated";
    name_opPostCopyGenerated = "opPostCopyGenerated";
    name_opPostMoveGenerated = "opPostMoveGenerated";

    name_Safety     = "Safety";
    name_Optim      = "Optim";
    name_Match      = "Match";
    name_Pack       = "Pack";
    name_Align      = "Align";
    name_Complete   = "Complete";
    name_AttrUsage  = "AttrUsage";
    name_AttrMulti  = "AttrMulti";
    name_Global     = "Global";
    name_Strict     = "Strict";
    name_PrintBc    = "PrintBc";
    name_ExportType = "ExportType";
    name_Using      = "Using";
    name_Warn       = "Warn";

    name_Swag_AttrUsage  = "Swag.AttrUsage";
    name_Swag_AttrMulti  = "Swag.AttrMulti";
    name_Swag_Align      = "Swag.Align";
    name_Swag_Safety     = "Swag.Safety";
    name_Swag_Optim      = "Swag.Optim";
    name_Swag_Match      = "Swag.Match";
    name_Swag_Pack       = "Swag.Pack";
    name_Swag_Foreign    = "Swag.Foreign";
    name_Swag_Deprecated = "Swag.Deprecated";
    name_Swag_Opaque     = "Swag.Opaque";
    name_Swag_Offset     = "Swag.Offset";
    name_Swag_ExportType = "Swag.ExportType";
    name_Swag_Warn       = "Swag.Warn";
    name_Swag_ConstExpr  = "Swag.ConstExpr";

    name_opBinary       = "opBinary";
    name_opUnary        = "opUnary";
    name_opAssign       = "opAssign";
    name_opIndexAssign  = "opIndexAssign";
    name_opCast         = "opCast";
    name_opEquals       = "opEquals";
    name_opCmp          = "opCmp";
    name_opPostCopy     = "opPostCopy";
    name_opPostMove     = "opPostMove";
    name_opDrop         = "opDrop";
    name_opCount        = "opCount";
    name_opData         = "opData";
    name_opAffect       = "opAffect";
    name_opAffectSuffix = "opAffectSuffix";
    name_opSlice        = "opSlice";
    name_opIndex        = "opIndex";
    name_opIndexAffect  = "opIndexAffect";
    name_opInit         = "opInit";
    name_opVisit        = "opVisit";

    name_opInitCrc         = name_opInit.hash();
    name_opDropCrc         = name_opDrop.hash();
    name_opPostCopyCrc     = name_opPostCopy.hash();
    name_opPostMoveCrc     = name_opPostMove.hash();
    name_opCastCrc         = name_opCast.hash();
    name_opAffectCrc       = name_opAffect.hash();
    name_opAffectSuffixCrc = name_opAffectSuffix.hash();

    name_TypeInfo          = "TypeInfo";
    name_TypeInfoNative    = "TypeInfoNative";
    name_TypeInfoPointer   = "TypeInfoPointer";
    name_TypeInfoStruct    = "TypeInfoStruct";
    name_TypeInfoParam     = "TypeInfoParam";
    name_TypeInfoFunc      = "TypeInfoFunc";
    name_TypeInfoEnum      = "TypeInfoEnum";
    name_TypeInfoVariadic  = "TypeInfoVariadic";
    name_TypeInfoArray     = "TypeInfoArray";
    name_TypeInfoSlice     = "TypeInfoSlice";
    name_TypeInfoGeneric   = "TypeInfoGeneric";
    name_TypeInfoAlias     = "TypeInfoAlias";
    name_TypeInfoNamespace = "TypeInfoNamespace";

    name_SystemAllocator      = "SystemAllocator";
    name_DebugAllocator       = "DebugAllocator";
    name_SourceCodeLocation   = "SourceCodeLocation";
    name_BuildCfg             = "BuildCfg";
    name_AttributeUsage       = "AttributeUsage";
    name_CVaList              = "CVaList";
    name_TargetArch           = "TargetArch";
    name_TargetOs             = "TargetOs";
    name_Backend              = "Backend";
    name_Swag_CompilerMsgMask = "Swag.CompilerMsgMask";

    name_data      = "data";
    name_count     = "count";
    name_sizeof    = "sizeof";
    name_string    = "string";
    name_itable    = "itable";
    name_interface = "interface";
    name_ptrvalue  = "ptrvalue";
    name_typeinfo  = "typeinfo";

    name_export      = "export";
    name_generated   = "generated";
    name_skip        = "skip";
    name_testpass    = "testpass";
    name_lexer       = "lexer";
    name_syntax      = "syntax";
    name_semantic    = "semantic";
    name_backend     = "backend";
    name_testerror   = "testerror";
    name_testwarning = "testwarning";
    name_location    = "location";
    name_version     = "version";

    name_up      = "up";
    name_safe    = "safe";
    name_small   = "small";
    name_nodrop  = "nodrop";
    name_move    = "move";
    name_moveraw = "moveraw";
    name_err     = "err";
    name_noerr   = "noerr";
    name_bit     = "bit";

    name_bound  = "bound";
    name_over   = "over";
    name_switch = "switch";
    name_math   = "math";
    name_any    = "any";
    name_bool   = "bool";
    name_nan    = "nan";
    name_sanity = "sanity";

    name_name     = "name";
    name_msg      = "msg";
    name_module   = "module";
    name_function = "function";
    name_bytecode = "bytecode";
    name_validif  = "validif";
    name_usage    = "usage";
    name_what     = "what";
    name_value    = "value";
    name_level    = "level";
    name_disk     = "disk";
    name_retval   = "retval";
    name_methods  = "methods";
    name_nozero   = "nozero";
    name_toplevel = "toplevel";

    name_atalias           = "@alias";
    name_atmixin           = "@mixin";
    name_atdrop            = "@drop";
    name_atpostCopy        = "@postCopy";
    name_atpostMove        = "@postMove";
    name_atargs            = "@args";
    name_atpanic           = "@panic";
    name_atcompilererror   = "@compilererror";
    name_atcompilerwarning = "@compilerwarning";
    name_atitftableof      = "@itftableof";
    name_atstrcmp          = "@stringcmp";
    name_attypecmp         = "@typecmp";
    name_atdbgalloc        = "@dbgalloc";
    name_atsysalloc        = "@sysalloc";
    name_atrtflags         = "@rtflags";

    name_sharpself = "#self";

    name_memcpy  = "memcpy";
    name_memset  = "memset";
    name_memmove = "memmove";
    name_memcmp  = "memcmp";
    name_strlen  = "strlen";
    name_strcmp  = "strcmp";
    name_malloc  = "malloc";
    name_realloc = "realloc";
    name_free    = "free";
    name_powf    = "powf";
    name_atan2f  = "atan2f";
    name_pow     = "pow";
    name_atan2   = "atan2";
    name_sinf    = "sinf";
    name_cosf    = "cosf";
    name_tanf    = "tanf";
    name_sinhf   = "sinhf";
    name_coshf   = "coshf";
    name_tanhf   = "tanhf";
    name_asinf   = "asinf";
    name_acosf   = "acosf";
    name_atanf   = "atanf";
    name_logf    = "logf";
    name_log2f   = "log2f";
    name_log10f  = "log10f";
    name_floorf  = "floorf";
    name_ceilf   = "ceilf";
    name_truncf  = "truncf";
    name_roundf  = "roundf";
    name_expf    = "expf";
    name_exp2f   = "exp2f";
    name_sin     = "sin";
    name_cos     = "cos";
    name_tan     = "tan";
    name_sinh    = "sinh";
    name_cosh    = "cosh";
    name_tanh    = "tanh";
    name_asin    = "asin";
    name_acos    = "acos";
    name_atan    = "atan";
    name_log     = "log";
    name_log2    = "log2";
    name_log10   = "log10";
    name_floor   = "floor";
    name_ceil    = "ceil";
    name_trunc   = "trunc";
    name_round   = "round";
    name_exp     = "exp";
    name_exp2    = "exp2";
}

void LanguageSpec::setupKeywords()
{
    keywords.add("#global", TokenId::CompilerGlobal);

    keywords.add("#run", TokenId::CompilerRun);
    keywords.add("#ast", TokenId::CompilerAst);
    keywords.add("#test", TokenId::CompilerFuncTest);
    keywords.add("#init", TokenId::CompilerFuncInit);
    keywords.add("#drop", TokenId::CompilerFuncDrop);
    keywords.add("#main", TokenId::CompilerFuncMain);
    keywords.add("#premain", TokenId::CompilerFuncPreMain);
    keywords.add("#message", TokenId::CompilerFuncCompiler);
    keywords.add("#dependencies", TokenId::CompilerDependencies);

    keywords.add("#include", TokenId::CompilerInclude);
    keywords.add("#load", TokenId::CompilerLoad);
    keywords.add("#assert", TokenId::CompilerAssert);
    keywords.add("#print", TokenId::CompilerPrint);
    keywords.add("#error", TokenId::CompilerError);
    keywords.add("#warning", TokenId::CompilerWarning);
    keywords.add("#foreignlib", TokenId::CompilerForeignLib);
    keywords.add("#import", TokenId::CompilerImport);
    keywords.add("#mixin", TokenId::CompilerMixin);
    keywords.add("#macro", TokenId::CompilerMacro);
    keywords.add("#placeholder", TokenId::CompilerPlaceHolder);
    keywords.add("#if", TokenId::CompilerIf);
    keywords.add("#else", TokenId::CompilerElse);
    keywords.add("#elif", TokenId::CompilerElseIf);
    keywords.add("#validif", TokenId::CompilerValidIf);
    keywords.add("#validifx", TokenId::CompilerValidIfx);

    keywords.add("#cfg", TokenId::CompilerBuildCfg);
    keywords.add("#os", TokenId::CompilerOs);
    keywords.add("#arch", TokenId::CompilerArch);
    keywords.add("#cpu", TokenId::CompilerCpu);
    keywords.add("#backend", TokenId::CompilerBackend);
    keywords.add("#module", TokenId::CompilerModule);
    keywords.add("#file", TokenId::CompilerFile);
    keywords.add("#line", TokenId::CompilerLine);
    keywords.add("#self", TokenId::CompilerSelf);
    keywords.add("#location", TokenId::CompilerLocation);
    keywords.add("#callerlocation", TokenId::CompilerCallerLocation);
    keywords.add("#callerfunction", TokenId::CompilerCallerFunction);
    keywords.add("#swagversion", TokenId::CompilerBuildVersion);
    keywords.add("#swagrevision", TokenId::CompilerBuildRevision);
    keywords.add("#swagbuildnum", TokenId::CompilerBuildNum);
    keywords.add("#swagos", TokenId::CompilerSwagOs);
    keywords.add("#code", TokenId::CompilerCode);
    keywords.add("#type", TokenId::CompilerType);
    keywords.add("#scope", TokenId::CompilerScope);
    keywords.add("#scopefile", TokenId::CompilerScopeFile);
    keywords.add("#up", TokenId::CompilerUp);

    keywords.add("true", TokenId::KwdTrue);
    keywords.add("false", TokenId::KwdFalse);
    keywords.add("null", TokenId::KwdNull);
    keywords.add("undefined", TokenId::KwdUndefined);

    keywords.add("using", TokenId::KwdUsing);
    keywords.add("with", TokenId::KwdWith);
    keywords.add("cast", TokenId::KwdCast);
    keywords.add("acast", TokenId::KwdAutoCast);
    keywords.add("dref", TokenId::KwdDeRef);
    keywords.add("retval", TokenId::KwdRetVal);
    keywords.add("try", TokenId::KwdTry);
    keywords.add("trycatch", TokenId::KwdTryCatch);
    keywords.add("catch", TokenId::KwdCatch);
    keywords.add("assume", TokenId::KwdAssume);
    keywords.add("throw", TokenId::KwdThrow);
    keywords.add("discard", TokenId::KwdDiscard);

    keywords.add("public", TokenId::KwdPublic);
    keywords.add("private", TokenId::KwdPrivate);

    keywords.add("enum", TokenId::KwdEnum);
    keywords.add("struct", TokenId::KwdStruct);
    keywords.add("union", TokenId::KwdUnion);
    keywords.add("impl", TokenId::KwdImpl);
    keywords.add("interface", TokenId::KwdInterface);
    keywords.add("func", TokenId::KwdFunc);
    keywords.add("closure", TokenId::KwdClosure);
    keywords.add("mtd", TokenId::KwdMethod);
    keywords.add("mtdc", TokenId::KwdConstMethod);
    keywords.add("namespace", TokenId::KwdNamespace);
    keywords.add("alias", TokenId::KwdAlias);
    keywords.add("attr", TokenId::KwdAttr);
    keywords.add("var", TokenId::KwdVar);
    keywords.add("const", TokenId::KwdConst);
    keywords.add("ref", TokenId::KwdRef);
    keywords.add("moveref", TokenId::KwdMoveRef);

    keywords.add("if", TokenId::KwdIf);
    keywords.add("else", TokenId::KwdElse);
    keywords.add("elif", TokenId::KwdElif);
    keywords.add("for", TokenId::KwdFor);
    keywords.add("while", TokenId::KwdWhile);
    keywords.add("switch", TokenId::KwdSwitch);
    keywords.add("defer", TokenId::KwdDefer);
    keywords.add("loop", TokenId::KwdLoop);
    keywords.add("visit", TokenId::KwdVisit);
    keywords.add("break", TokenId::KwdBreak);
    keywords.add("fallthrough", TokenId::KwdFallThrough);
    keywords.add("return", TokenId::KwdReturn);
    keywords.add("case", TokenId::KwdCase);
    keywords.add("continue", TokenId::KwdContinue);
    keywords.add("default", TokenId::KwdDefault);
    keywords.add("and", TokenId::KwdAnd);
    keywords.add("or", TokenId::KwdOr);
    keywords.add("orelse", TokenId::KwdOrElse);

    keywords.add("@defined", TokenId::IntrinsicDefined);
    keywords.add("@location", TokenId::IntrinsicLocation);
    keywords.add("@stringcmp", TokenId::IntrinsicStringCmp);
    keywords.add("@typecmp", TokenId::IntrinsicTypeCmp);
    keywords.add("@getcontext", TokenId::IntrinsicGetContext);
    keywords.add("@setcontext", TokenId::IntrinsicSetContext);
    keywords.add("@args", TokenId::IntrinsicArguments);
    keywords.add("@compiler", TokenId::IntrinsicCompiler);
    keywords.add("@isbytecode", TokenId::IntrinsicIsByteCode);
    keywords.add("@print", TokenId::IntrinsicPrint);
    keywords.add("@compilererror", TokenId::IntrinsicCompilerError);
    keywords.add("@compilerwarning", TokenId::IntrinsicCompilerWarning);
    keywords.add("@breakpoint", TokenId::IntrinsicBcBreakpoint);
    keywords.add("@assert", TokenId::IntrinsicAssert);
    keywords.add("@panic", TokenId::IntrinsicPanic);
    keywords.add("@hastag", TokenId::IntrinsicHasTag);
    keywords.add("@gettag", TokenId::IntrinsicGetTag);
    keywords.add("@init", TokenId::IntrinsicInit);
    keywords.add("@drop", TokenId::IntrinsicDrop);
    keywords.add("@safety", TokenId::IntrinsicSafety);
    keywords.add("@spread", TokenId::IntrinsicSpread);
    keywords.add("@postmove", TokenId::IntrinsicPostMove);
    keywords.add("@postcopy", TokenId::IntrinsicPostCopy);
    keywords.add("@sizeof", TokenId::IntrinsicSizeOf);
    keywords.add("@alignof", TokenId::IntrinsicAlignOf);
    keywords.add("@offsetof", TokenId::IntrinsicOffsetOf);
    keywords.add("@typeof", TokenId::IntrinsicTypeOf);
    keywords.add("@kindof", TokenId::IntrinsicKindOf);
    keywords.add("@decltype", TokenId::IntrinsicDeclType);
    keywords.add("@countof", TokenId::IntrinsicCountOf);
    keywords.add("@stringof", TokenId::IntrinsicStringOf);
    keywords.add("@nameof", TokenId::IntrinsicNameOf);
    keywords.add("@runes", TokenId::IntrinsicRunes);
    keywords.add("@dataof", TokenId::IntrinsicDataOf);
    keywords.add("@mkslice", TokenId::IntrinsicMakeSlice);
    keywords.add("@mkstring", TokenId::IntrinsicMakeString);
    keywords.add("@mkany", TokenId::IntrinsicMakeAny);
    keywords.add("@mkinterface", TokenId::IntrinsicMakeInterface);
    keywords.add("@mkcallback", TokenId::IntrinsicMakeCallback);
    keywords.add("@index", TokenId::IntrinsicIndex);
    keywords.add("@isconstexpr", TokenId::IntrinsicIsConstExpr);
    keywords.add("@err", TokenId::IntrinsicGetErr);
    keywords.add("@itftableof", TokenId::IntrinsicItfTableOf);
    keywords.add("@dbgalloc", TokenId::IntrinsicDbgAlloc);
    keywords.add("@sysalloc", TokenId::IntrinsicSysAlloc);
    keywords.add("@rtflags", TokenId::IntrinsicRtFlags);
    keywords.add("@getpinfos", TokenId::IntrinsicGetProcessInfos);
    keywords.add("@modules", TokenId::IntrinsicModules);
    keywords.add("@gvtd", TokenId::IntrinsicGvtd);

    keywords.add("@byteswap", TokenId::IntrinsicByteSwap);
    keywords.add("@bitcountnz", TokenId::IntrinsicBitCountNz);
    keywords.add("@bitcounttz", TokenId::IntrinsicBitCountTz);
    keywords.add("@bitcountlz", TokenId::IntrinsicBitCountLz);
    keywords.add("@min", TokenId::IntrinsicMin);
    keywords.add("@max", TokenId::IntrinsicMax);
    keywords.add("@sqrt", TokenId::IntrinsicSqrt);
    keywords.add("@sin", TokenId::IntrinsicSin);
    keywords.add("@cos", TokenId::IntrinsicCos);
    keywords.add("@tan", TokenId::IntrinsicTan);
    keywords.add("@sinh", TokenId::IntrinsicSinh);
    keywords.add("@cosh", TokenId::IntrinsicCosh);
    keywords.add("@tanh", TokenId::IntrinsicTanh);
    keywords.add("@asin", TokenId::IntrinsicASin);
    keywords.add("@acos", TokenId::IntrinsicACos);
    keywords.add("@atan", TokenId::IntrinsicATan);
    keywords.add("@atan2", TokenId::IntrinsicATan2);
    keywords.add("@log", TokenId::IntrinsicLog);
    keywords.add("@log2", TokenId::IntrinsicLog2);
    keywords.add("@log10", TokenId::IntrinsicLog10);
    keywords.add("@floor", TokenId::IntrinsicFloor);
    keywords.add("@ceil", TokenId::IntrinsicCeil);
    keywords.add("@trunc", TokenId::IntrinsicTrunc);
    keywords.add("@round", TokenId::IntrinsicRound);
    keywords.add("@abs", TokenId::IntrinsicAbs);
    keywords.add("@exp", TokenId::IntrinsicExp);
    keywords.add("@exp2", TokenId::IntrinsicExp2);
    keywords.add("@pow", TokenId::IntrinsicPow);
    keywords.add("@alloc", TokenId::IntrinsicAlloc);
    keywords.add("@realloc", TokenId::IntrinsicRealloc);
    keywords.add("@free", TokenId::IntrinsicFree);
    keywords.add("@memcpy", TokenId::IntrinsicMemCpy);
    keywords.add("@memmove", TokenId::IntrinsicMemMove);
    keywords.add("@memset", TokenId::IntrinsicMemSet);
    keywords.add("@memcmp", TokenId::IntrinsicMemCmp);
    keywords.add("@muladd", TokenId::IntrinsicMulAdd);
    keywords.add("@strlen", TokenId::IntrinsicStrLen);
    keywords.add("@strcmp", TokenId::IntrinsicStrCmp);
    keywords.add("@atomadd", TokenId::IntrinsicAtomicAdd);
    keywords.add("@atomand", TokenId::IntrinsicAtomicAnd);
    keywords.add("@atomor", TokenId::IntrinsicAtomicOr);
    keywords.add("@atomxor", TokenId::IntrinsicAtomicXor);
    keywords.add("@atomxchg", TokenId::IntrinsicAtomicXchg);
    keywords.add("@atomcmpxchg", TokenId::IntrinsicAtomicCmpXchg);
    keywords.add("@cvastart", TokenId::IntrinsicCVaStart);
    keywords.add("@cvaend", TokenId::IntrinsicCVaEnd);
    keywords.add("@cvaarg", TokenId::IntrinsicCVaArg);

    keywords.add("code", TokenId::KwdCode);
    keywords.add("cvarargs", TokenId::KwdCVarArgs);

    keywords.add("any", TokenId::NativeType);
    keywords.add("void", TokenId::NativeType);
    keywords.add("rune", TokenId::NativeType);
    keywords.add("f32", TokenId::NativeType);
    keywords.add("f64", TokenId::NativeType);
    keywords.add("s8", TokenId::NativeType);
    keywords.add("s16", TokenId::NativeType);
    keywords.add("s32", TokenId::NativeType);
    keywords.add("s64", TokenId::NativeType);
    keywords.add("u8", TokenId::NativeType);
    keywords.add("u16", TokenId::NativeType);
    keywords.add("u32", TokenId::NativeType);
    keywords.add("u64", TokenId::NativeType);
    keywords.add("bool", TokenId::NativeType);
    keywords.add("string", TokenId::NativeType);
    keywords.add("cstring", TokenId::NativeType);
    keywords.add("typeinfo", TokenId::NativeType);
}

void LanguageSpec::setupNativeTypes()
{
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
    nativeTypes.add("cstring", LiteralType::TT_CSTRING);
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
    attributesFlags.add("NoInline", ATTRIBUTE_NO_INLINE);
    attributesFlags.add("Macro", ATTRIBUTE_MACRO);
    attributesFlags.add("Mixin", ATTRIBUTE_MIXIN);
    attributesFlags.add("Complete", ATTRIBUTE_COMPLETE);
    attributesFlags.add("Incomplete", ATTRIBUTE_INCOMPLETE);
    attributesFlags.add("Implicit", ATTRIBUTE_IMPLICIT);
    attributesFlags.add("NoReturn", ATTRIBUTE_NO_RETURN);
    attributesFlags.add("Discardable", ATTRIBUTE_DISCARDABLE);
    attributesFlags.add("Deprecated", ATTRIBUTE_DEPRECATED);
    attributesFlags.add("Global", ATTRIBUTE_GLOBAL);
    attributesFlags.add("Tls", ATTRIBUTE_TLS);
    attributesFlags.add("Strict", ATTRIBUTE_STRICT);
    attributesFlags.add("NoCopy", ATTRIBUTE_NO_COPY);
    attributesFlags.add("Opaque", ATTRIBUTE_OPAQUE);
    attributesFlags.add("NotGeneric", ATTRIBUTE_NOT_GENERIC);
}

void LanguageSpec::setup()
{
    setupNames();
    setupKeywords();
    setupNativeTypes();
    setupAttributesFlags();
}