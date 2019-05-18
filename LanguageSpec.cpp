#include "pch.h"
#include "LanguageSpec.h"

LanguageSpec::LanguageSpec()
{
    m_keywords[L"#unittest"] = TokenId::CompilerUnitTest;
    m_keywords[L"u64"]       = TokenId::NativeType;
    m_keywords[L"u32"]       = TokenId::NativeType;
    m_keywords[L"u16"]       = TokenId::NativeType;
    m_keywords[L"u8"]        = TokenId::NativeType;
    m_keywords[L"s64"]       = TokenId::NativeType;
    m_keywords[L"s32"]       = TokenId::NativeType;
    m_keywords[L"s16"]       = TokenId::NativeType;
    m_keywords[L"s8"]        = TokenId::NativeType;
    m_keywords[L"f32"]       = TokenId::NativeType;
    m_keywords[L"f64"]       = TokenId::NativeType;
    m_keywords[L"bool"]      = TokenId::NativeType;

    m_nativeTypes[L"u64"]  = TokenNumType::UInt64;
    m_nativeTypes[L"u32"]  = TokenNumType::UInt32;
    m_nativeTypes[L"u16"]  = TokenNumType::UInt16;
    m_nativeTypes[L"u8"]   = TokenNumType::UInt8;
    m_nativeTypes[L"s64"]  = TokenNumType::Int64;
    m_nativeTypes[L"s32"]  = TokenNumType::Int32;
    m_nativeTypes[L"s16"]  = TokenNumType::Int16;
    m_nativeTypes[L"s8"]   = TokenNumType::Int8;
    m_nativeTypes[L"f32"]  = TokenNumType::Float32;
    m_nativeTypes[L"f64"]  = TokenNumType::Float64;
    m_nativeTypes[L"bool"] = TokenNumType::Bool;
}
