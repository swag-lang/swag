#global namespace Serialization

public interface ISerialize
{
    read:  func(self, IDecoder) throw
    write: func(self, IEncoder)
}

public struct(T) Serializer
{
    #assert @typeof(T) == IDecoder or @typeof(T) == IEncoder
    ser: T
}

public impl Serializer
{
    mtd(MT) serialize(name: string, res: *MT) throw
    {
        #if T == IDecoder
        {
            #if MT == s32
                dref res = ser.readS32(name)
            #else
                ser.read(name, res, @typeof(MT))
        }
        #else
        {
            #if MT == s32
                ser.writeS32(name, dref res)
            #else
                ser.write(name, @typeof(MT), res)
        }
    }

    mtd serializeBufferU8(name: string, res: **u8, count: uint, allocator: Swag.IAllocator) throw
    {
        #if T == IDecoder
        {
        }
        #else
        {
            ser.writeBufferU8(name, dref res, count)
        }
    }
}