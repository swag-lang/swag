#pragma once
enum class ByteCodeNodeId : uint16_t
{
    End = 0,

	DebugInfos,

	LocalFuncCall,
    Ret,
	IntrinsicPrintS64,
    IntrinsicPrintF64,
    IntrinsicPrintChar,
    IntrinsicPrintString,

    PushBool,
    PushS8,
    PushS16,
    PushS32,
    PushS64,
    PushU8,
    PushU16,
    PushU32,
    PushU64,
    PushF32,
    PushF64,
	PushString,

    BinOpPlusS32,
    BinOpPlusS64,
    BinOpPlusU32,
    BinOpPlusU64,
    BinOpPlusF32,
    BinOpPlusF64,

    BinOpMinusS32,
    BinOpMinusS64,
    BinOpMinusU32,
    BinOpMinusU64,
    BinOpMinusF32,
    BinOpMinusF64,

    BinOpMulS32,
    BinOpMulS64,
    BinOpMulU32,
    BinOpMulU64,
    BinOpMulF32,
    BinOpMulF64,

	BinOpDivF32,
	BinOpDivF64,
};