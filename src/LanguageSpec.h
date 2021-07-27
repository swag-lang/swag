#pragma once
#pragma once
#include "TypeInfo.h"
#include "Tokenizer.h"
#include "Utf8.h"
#include "HashTable.h"
struct TypeInfoNative;

struct LanguageSpec
{
    void   setup();
    string tokenToName(TokenId id);

    HashTable<pair<TokenId, LiteralType>, 4096> keywords;
    HashTable<LiteralType, 256>                 nativeTypes;

    Utf8 name_Swag;
    Utf8 name_self;
    Utf8 name_Self;
    Utf8 name_SystemAllocator;
    Utf8 name_Complete;
    Utf8 name_AttrUsage;
    Utf8 name_AttrMulti;
    Utf8 name_Global;
    Utf8 name_Strict;
    Utf8 name_PrintBc;
    Utf8 name_Align;
    Utf8 name_opInitGenerated;
    Utf8 name_opDropGenerated;
    Utf8 name_opRelocGenerated;
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
    Utf8 name_opSlice;
    Utf8 name_opIndex;
    Utf8 name_opIndexAffect;
    Utf8 name_opInit;
    Utf8 name_TypeInfo;
    Utf8 name_TypeInfoNative;
    Utf8 name_TypeInfoPointer;
    Utf8 name_TypeInfoReference;
    Utf8 name_TypeInfoStruct;
    Utf8 name_TypeInfoParam;
    Utf8 name_TypeInfoFunc;
    Utf8 name_TypeInfoEnum;
    Utf8 name_TypeInfoVariadic;
    Utf8 name_TypeInfoArray;
    Utf8 name_TypeInfoSlice;
    Utf8 name_TypeInfoGeneric;
    Utf8 name_TypeInfoAlias;
    Utf8 name_CompilerSourceLocation;
    Utf8 name_BuildCfg;
    Utf8 name_export;
    Utf8 name_generated;
    Utf8 name_foreignlib;
    Utf8 name_skip;
    Utf8 name_testpass;
    Utf8 name_lexer;
    Utf8 name_syntax;
    Utf8 name_semantic;
    Utf8 name_backend;
    Utf8 name_testerror;
    Utf8 name_testerrors;
    Utf8 name_testwarning;
    Utf8 name_testwarnings;
    Utf8 name_location;
    Utf8 name_version;
    Utf8 name_AttributeUsage;
    Utf8 name_swag;
    Utf8 name_disk;
    Utf8 name_atalias;
    Utf8 name_safe;
    Utf8 name_small;
    Utf8 name_nodrop;
    Utf8 name_move;
    Utf8 name_moveraw;
    Utf8 name_Swag_CompilerMsgMask;
    Utf8 name_retval;
};

extern LanguageSpec g_LangSpec;