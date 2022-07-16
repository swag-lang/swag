#global namespace Serialization.Write
#global public
using Swag

struct BinOptions
{
}

struct Bin
{
    options:    BinOptions
    output:     *ConcatBuffer
}

impl Bin
{
    mtd startWrite(buf: *ConcatBuffer)
    {
        output = buf
    }

    mtd beginRoot()
    {
    }

    mtd endRoot()
    {
    }

    mtd beginSequence(typeElem: typeinfo, data: const *void, countElem: uint)->bool
    {
        output.addNative(countElem)
        return false
    }

    mtd endSequence()
    {
    }

    mtd beginStruct(type: typeinfo, data: const *u8)->bool
    {
        return false
    }

    mtd endStruct()
    {
    }

    mtd beginElement(name: string, type: typeinfo)
    {
    }

    mtd endElement()
    {
    }

    mtd beginValue(type: typeinfo)
    {
    }

    mtd endValue()
    {
    }

    mtd writeBool(value: bool)
    {
        output.addNative(value)
    }

    mtd writeF32(value: f32)
    {
        output.addNative(value)
    }

    mtd writeF64(value: f64)
    {
        output.addNative(value)
    }

    mtd writeU8(value: u8)
    {
        output.addNative(value)
    }

    mtd writeU16(value: u16)
    {
        output.addNative(value)
    }

    mtd writeU32(value: u32)
    {
        output.addNative(value)
    }

    mtd writeU64(value: u64)
    {
        output.addNative(value)
    }

    mtd writeS8(value: s8)
    {
        output.addNative(value)
    }

    mtd writeS16(value: s16)
    {
        output.addNative(value)
    }

    mtd writeS32(value: s32)
    {
        output.addNative(value)
    }

    mtd writeS64(value: s64)
    {
        output.addNative(value)
    }

    mtd writeString(value: string)
    {
        count := cast(u32) @countof(value)
        output.addNative(count)
        output.addBytes(value)
    }
}