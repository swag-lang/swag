#pragma once
#include "Backend/ByteCode/Register.h"

struct ByteCode;
struct ByteCodeInstruction;

enum class SanityValueKind : uint8_t
{
    Invalid,
    StackAddr,
    ConstantAddr,
    Constant,
    Unknown,
};

using SanityValueFlags = Flags<uint64_t>;

constexpr SanityValueFlags SANITY_VALUE_FLAG_NONE     = 0x0000000000000000;
constexpr SanityValueFlags SANITY_VALUE_FLAG_NOT_ZERO = 0x0000000000000001;

struct SanityValue
{
    VectorNative<ByteCodeInstruction*> ips;
    Register                           reg;
    SanityValueFlags                   flags = SANITY_VALUE_FLAG_NONE;
    SanityValueKind                    kind  = SanityValueKind::Invalid;

    bool isZero() const;
    bool isNotZero() const;
    void setConstant(int8_t val);
    void setConstant(int16_t val);
    void setConstant(int32_t val);
    void setConstant(int64_t val);
    void setConstant(uint8_t val);
    void setConstant(uint16_t val);
    void setConstant(uint32_t val);
    void setConstant(uint64_t val);
    void setConstant(float val);
    void setConstant(double val);
    void setUnknown(SanityValueFlags fl = SANITY_VALUE_FLAG_NONE);
    void setStackAddr(uint64_t val);
    void setConstantAddr(uint64_t val);
    void setKind(SanityValueKind val);

    bool isConstant() const { return kind == SanityValueKind::Constant; }
    bool isStackAddr() const { return kind == SanityValueKind::StackAddr; }
    bool isConstantAddr() const { return kind == SanityValueKind::ConstantAddr; }
    bool isUnknown() const { return kind == SanityValueKind::Unknown; }
    bool isTrueUnknown() const { return kind == SanityValueKind::Unknown && flags == SANITY_VALUE_FLAG_NONE; }

    static void setIps(ByteCodeInstruction* ip, SanityValue* ra = nullptr, SanityValue* rb = nullptr, SanityValue* rc = nullptr, SanityValue* rd = nullptr);
};
