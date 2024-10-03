#pragma once
#include "Backend/ByteCode/Register.h"
#include "Threading/Job.h"

struct ByteCode;
struct ByteCodeInstruction;

enum class SanityValueKind : uint8_t
{
    Invalid,
    StackAddr,
    Constant,
    Unknown,
    NotZero,
};

struct SanityValue
{
    VectorNative<ByteCodeInstruction*> ips;
    Register                           reg;
    SanityValueKind                    kind = SanityValueKind::Invalid;

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
    void setUnknown();
    void setStackAddr(uint64_t val);
    void setKind(SanityValueKind val);

    bool isConstant() const { return kind == SanityValueKind::Constant; }
    bool isStackAddr() const { return kind == SanityValueKind::StackAddr; }
    bool isUnknown() const { return kind == SanityValueKind::Unknown; }

    static void computeIp(ByteCodeInstruction* ip, SanityValue* ra = nullptr, SanityValue* rb = nullptr, SanityValue* rc = nullptr, SanityValue* rd = nullptr);
};
