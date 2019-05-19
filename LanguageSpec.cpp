#include "pch.h"
#include "LanguageSpec.h"

LanguageSpec::LanguageSpec()
{
    m_keywords["#unittest"] = TokenId::CompilerUnitTest;
    m_keywords["u64"]       = TokenId::NativeType;
    m_keywords["u32"]       = TokenId::NativeType;
    m_keywords["u16"]       = TokenId::NativeType;
    m_keywords["u8"]        = TokenId::NativeType;
    m_keywords["s64"]       = TokenId::NativeType;
    m_keywords["s32"]       = TokenId::NativeType;
    m_keywords["s16"]       = TokenId::NativeType;
    m_keywords["s8"]        = TokenId::NativeType;
    m_keywords["f32"]       = TokenId::NativeType;
    m_keywords["f64"]       = TokenId::NativeType;
    m_keywords["boo"]       = TokenId::NativeType;

    m_nativeTypes["u64"] = TokenNumType::UInt64;
    m_nativeTypes["u32"] = TokenNumType::UInt32;
    m_nativeTypes["u16"] = TokenNumType::UInt16;
    m_nativeTypes["u8"]  = TokenNumType::UInt8;
    m_nativeTypes["s64"] = TokenNumType::Int64;
    m_nativeTypes["s32"] = TokenNumType::Int32;
    m_nativeTypes["s16"] = TokenNumType::Int16;
    m_nativeTypes["s8"]  = TokenNumType::Int8;
    m_nativeTypes["f32"] = TokenNumType::Float32;
    m_nativeTypes["f64"] = TokenNumType::Float64;
    m_nativeTypes["boo"] = TokenNumType::Bool;
}
