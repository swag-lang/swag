#pragma once
#include "TypeInfo.h"
#include "Tokenizer.h"
#include "Utf8.h"
#include "LangHashTable.h"
struct TypeInfoNative;

struct LanguageSpec
{
    void setup();
    void setupNames();
    void setupKeywords();
    void setupNativeTypes();
    void setupAttributesFlags();

    LangHashTable<TokenId, 4096>   keywords;
    LangHashTable<LiteralType, 64> nativeTypes;
    LangHashTable<uint64_t, 256>   attributesFlags;

    Utf8 name_globalInit;
    Utf8 name_globalDrop;
    Utf8 name_globalPreMain;
    Utf8 name_getTypeTable;

    Utf8 name__setupRuntime;
    Utf8 name__closeRuntime;
    Utf8 name__dropGlobalVariables;
    Utf8 name__compilererror;
    Utf8 name__panic;
    Utf8 name__pusherr;
    Utf8 name__poperr;
    Utf8 name__geterrmsg;
    Utf8 name__seterr;
    Utf8 name__tlsAlloc;
    Utf8 name__tlsGetValue;
    Utf8 name__tlsSetValue;
    Utf8 name__tlsGetPtr;
    Utf8 name__initStackTrace;
    Utf8 name__stackTrace;
    Utf8 name__loaddll;

    Utf8 name_memcpy;
    Utf8 name_memset;
    Utf8 name_memmove;
    Utf8 name_memcmp;
    Utf8 name_strlen;
    Utf8 name_strcmp;
    Utf8 name_malloc;
    Utf8 name_realloc;
    Utf8 name_free;

    Utf8 name_swag;
    Utf8 name_Swag;
    Utf8 name_self;
    Utf8 name_Self;

    Utf8 name_Complete;
    Utf8 name_AttrUsage;
    Utf8 name_AttrMulti;
    Utf8 name_Global;
    Utf8 name_Strict;
    Utf8 name_PrintBc;
    Utf8 name_Align;
    Utf8 name_Overflow;
    Utf8 name_Safety;
    Utf8 name_Optim;
    Utf8 name_Match;
    Utf8 name_Pack;
    Utf8 name_ExportType;
    Utf8 name_Using;

    Utf8 name_Swag_AttrUsage;
    Utf8 name_Swag_AttrMulti;
    Utf8 name_Swag_Align;
    Utf8 name_Swag_Overflow;
    Utf8 name_Swag_Safety;
    Utf8 name_Swag_Optim;
    Utf8 name_Swag_Match;
    Utf8 name_Swag_Pack;
    Utf8 name_Swag_Foreign;
    Utf8 name_Swag_Deprecated;
    Utf8 name_Swag_Opaque;
    Utf8 name_Swag_Offset;
    Utf8 name_Swag_ExportType;
    Utf8 name_Swag_Warn;
    Utf8 name_Swag_ConstExpr;

    Utf8 name_opInitGenerated;
    Utf8 name_opDropGenerated;
    Utf8 name_opPostCopyGenerated;
    Utf8 name_opPostMoveGenerated;
    Utf8 name_opBinary;
    Utf8 name_opUnary;
    Utf8 name_opAssign;
    Utf8 name_opIndexAssign;
    Utf8 name_opCast;
    Utf8 name_opEquals;
    Utf8 name_opCmp;
    Utf8 name_opPostCopy;
    Utf8 name_opPostMove;
    Utf8 name_opDrop;
    Utf8 name_opCount;
    Utf8 name_opData;
    Utf8 name_opAffect;
    Utf8 name_opAffectSuffix;
    Utf8 name_opSlice;
    Utf8 name_opIndex;
    Utf8 name_opIndexAffect;
    Utf8 name_opInit;
    Utf8 name_opVisit;

    uint32_t name_opInitCrc;
    uint32_t name_opDropCrc;
    uint32_t name_opPostCopyCrc;
    uint32_t name_opPostMoveCrc;
    uint32_t name_opCastCrc;
    uint32_t name_opAffectCrc;
    uint32_t name_opAffectSuffixCrc;

    Utf8 name_TypeInfo;
    Utf8 name_TypeInfoNative;
    Utf8 name_TypeInfoPointer;
    Utf8 name_TypeInfoStruct;
    Utf8 name_TypeInfoParam;
    Utf8 name_TypeInfoFunc;
    Utf8 name_TypeInfoEnum;
    Utf8 name_TypeInfoVariadic;
    Utf8 name_TypeInfoArray;
    Utf8 name_TypeInfoSlice;
    Utf8 name_TypeInfoGeneric;
    Utf8 name_TypeInfoAlias;
    Utf8 name_TypeInfoNamespace;

    Utf8 name_SourceCodeLocation;
    Utf8 name_SystemAllocator;
    Utf8 name_DebugAllocator;
    Utf8 name_BuildCfg;
    Utf8 name_AttributeUsage;
    Utf8 name_Swag_CompilerMsgMask;
    Utf8 name_CVaList;
    Utf8 name_TargetArch;
    Utf8 name_TargetOs;
    Utf8 name_Backend;

    Utf8 name_data;
    Utf8 name_count;
    Utf8 name_sizeof;
    Utf8 name_string;
    Utf8 name_itable;
    Utf8 name_interface;
    Utf8 name_ptrvalue;
    Utf8 name_typeinfo;

    Utf8 name_export;
    Utf8 name_generated;
    Utf8 name_skip;
    Utf8 name_testpass;
    Utf8 name_lexer;
    Utf8 name_syntax;
    Utf8 name_semantic;
    Utf8 name_backend;
    Utf8 name_testerror;
    Utf8 name_testwarning;
    Utf8 name_location;
    Utf8 name_version;

    Utf8 name_up;
    Utf8 name_over;
    Utf8 name_nodrop;
    Utf8 name_move;
    Utf8 name_moveraw;
    Utf8 name_err;
    Utf8 name_noerr;
    Utf8 name_bit;
    Utf8 name_unconst;
    Utf8 name_back;

    Utf8 name_boundcheck;
    Utf8 name_overflow;
    Utf8 name_math;
    Utf8 name_switch;
    Utf8 name_unreachable;
    Utf8 name_bool;
    Utf8 name_nan;
    Utf8 name_any;
    Utf8 name_sanity;
    Utf8 name_null;

    Utf8 name_name;
    Utf8 name_msg;
    Utf8 name_module;
    Utf8 name_function;
    Utf8 name_bytecode;
    Utf8 name_validif;
    Utf8 name_usage;
    Utf8 name_what;
    Utf8 name_value;
    Utf8 name_level;
    Utf8 name_disk;
    Utf8 name_retval;
    Utf8 name_methods;
    Utf8 name_nozero;
    Utf8 name_toplevel;

    Utf8 name_atalias;
    Utf8 name_atmixin;
    Utf8 name_atdrop;
    Utf8 name_atpostCopy;
    Utf8 name_atpostMove;
    Utf8 name_atargs;
    Utf8 name_atpanic;
    Utf8 name_atcompilererror;
    Utf8 name_atcompilerwarning;
    Utf8 name_atitftableof;
    Utf8 name_atstrcmp;
    Utf8 name_attypecmp;
    Utf8 name_atdbgalloc;
    Utf8 name_atsysalloc;
    Utf8 name_atrtflags;
    Utf8 name_aterr;

    Utf8 name_sharpself;

    Utf8 name_powf;
    Utf8 name_atan2f;
    Utf8 name_pow;
    Utf8 name_atan2;
    Utf8 name_sinf;
    Utf8 name_cosf;
    Utf8 name_tanf;
    Utf8 name_sinhf;
    Utf8 name_coshf;
    Utf8 name_tanhf;
    Utf8 name_asinf;
    Utf8 name_acosf;
    Utf8 name_atanf;
    Utf8 name_logf;
    Utf8 name_log2f;
    Utf8 name_log10f;
    Utf8 name_floorf;
    Utf8 name_ceilf;
    Utf8 name_truncf;
    Utf8 name_roundf;
    Utf8 name_expf;
    Utf8 name_exp2f;
    Utf8 name_sin;
    Utf8 name_cos;
    Utf8 name_tan;
    Utf8 name_sinh;
    Utf8 name_cosh;
    Utf8 name_tanh;
    Utf8 name_asin;
    Utf8 name_acos;
    Utf8 name_atan;
    Utf8 name_log;
    Utf8 name_log2;
    Utf8 name_log10;
    Utf8 name_floor;
    Utf8 name_ceil;
    Utf8 name_trunc;
    Utf8 name_round;
    Utf8 name_exp;
    Utf8 name_exp2;
};

extern LanguageSpec* g_LangSpec;