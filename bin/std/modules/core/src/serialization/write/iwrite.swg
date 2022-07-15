#global namespace Serialization

public interface IWrite
{
    beginElement:   func(self, string, typeinfo)
    endElement:     func(self)
    beginValue:     func(self, typeinfo)
    endValue:       func(self)
    beginSequence:  func(self, typeinfo, uint, const *u8)->bool
    endSequence:    func(self)
    beginStruct:    func(self, typeinfo, const *u8)->bool
    endStruct:      func(self)

    writeBool:      func(self, bool)
    writeF32:       func(self, f32)
    writeF64:       func(self, f64)
    writeU8:        func(self, u8)
    writeU16:       func(self, u16)
    writeU32:       func(self, u32)
    writeU64:       func(self, u64)
    writeS8:        func(self, s8)
    writeS16:       func(self, s16)
    writeS32:       func(self, s32)
    writeS64:       func(self, s64)
    writeString:    func(self, string)
}