#global namespace Serialization.Read
using Swag

public struct Bin
{
    stream: ByteStream
}

public impl Bin
{
    mtd startRead(input: const [..] u8)
    {
        stream.init(input)
    }

    mtd beginSequence(typeElem: typeinfo, data: *u8, countElem: *uint)->bool throw
    {
        dref countElem = stream.readNative'uint()
        return false
    }

    mtd endSequence() throw
    {
    }

    mtd toNextSequenceElement(countElem: *uint)->bool
    {
        if dref countElem == 0
            return false
        dref countElem -= 1
        return true
    }

    mtd beginRoot() throw
    {
    }

    mtd endRoot() throw
    {
    }

    mtd beginStruct(data: *u8, type: typeinfo)->bool throw
    {
        return false
    }

    mtd endStruct() throw
    {
    }

    mtd beginElement(name: string, type: typeinfo) throw
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

    /////////////////////////////////
    // VALUES
    /////////////////////////////////

    mtd readBool()->bool throw
    {
        return stream.readNative'bool()
    }

    mtd readF32()->f32 throw
    {
        return stream.readNative'f32()
    }

    mtd readF64()->f64 throw
    {
        return stream.readNative'f64()
    }

    mtd readS8()->s8 throw
    {
        return stream.readNative's8()
    }

    mtd readS16()->s16 throw
    {
        return stream.readNative's16()
    }


    mtd readS32()->s32 throw
    {
        return stream.readNative's32()
    }

    mtd readS64()->s64 throw
    {
        return stream.readNative's64()
    }

    mtd readU8()->u8 throw
    {
        return stream.readNative'u8()
    }

    mtd readU16()->u16 throw
    {
        return stream.readNative'u16()
    }


    mtd readU32()->u32 throw
    {
        return stream.readNative'u32()
    }

    mtd readU64()->u64 throw
    {
        return stream.readNative'u64()
    }

    mtd readString()->String throw
    {
        var result: retval
        count := cast(uint) stream.readNative'u32()
        result.reserve(count)
        stream.readBytes(result.buffer, count)
        result.length = count
        return result
    }
}