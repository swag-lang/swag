#global namespace Serialization.Write
#global public
using Swag

interface ISerializer
{
    startWrite:         func(self, *ConcatBuffer)
    startElement:       func(self, const *Swag.TypeValue)
    endElement:         func(self, const *Swag.TypeValue)
    startValue:         func(self, typeinfo)
    endValue:           func(self, typeinfo)
    startStruct:        func(self, typeinfo, const *u8)->bool
    endStruct:          func(self)
    startSequence:      func(self, typeinfo, uint, const *u8)->bool
    endSequence:        func(self)

    writeBool:          func(self, bool)
    writeF32:           func(self, f32)
    writeF64:           func(self, f64)
    writeU8:            func(self, u8)
    writeU16:           func(self, u16)
    writeU32:           func(self, u32)
    writeU64:           func(self, u64)
    writeS8:            func(self, s8)
    writeS16:           func(self, s16)
    writeS32:           func(self, s32)
    writeS64:           func(self, s64)
    writeString:        func(self, string)
}

struct Serializer
{
    output: *ConcatBuffer
}

impl ISerializer for Serializer
{
    mtd startWrite(buf: *ConcatBuffer)
    {
        output = buf
    }

    mtd startElement(elem: const *Swag.TypeValue) {}
    mtd endElement(elem: const *Swag.TypeValue) {}

    mtd startSequence(typeElem: typeinfo, countElem: uint, data: const *u8) => false
    mtd endSequence() {}

    mtd startStruct(type: typeinfo, data: const *u8) => false
    mtd endStruct() {}

    mtd startValue(type: typeinfo) {}
    mtd endValue(type: typeinfo) {}

    mtd writeBool(value: bool)      {}
    mtd writeF32(value: f32)        {}
    mtd writeF64(value: f64)        {}
    mtd writeU8(value: u8)          {}
    mtd writeU16(value: u16)        {}
    mtd writeU32(value: u32)        {}
    mtd writeU64(value: u64)        {}
    mtd writeS8(value: s8)          {}
    mtd writeS16(value: s16)        {}
    mtd writeS32(value: s32)        {}
    mtd writeS64(value: s64)        {}
    mtd writeString(value: string)  {}
}

impl Serializer
{
    mtd initWrite(buffer: *ConcatBuffer)
    {
        output = buffer
    }
}