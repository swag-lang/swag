#global namespace Serialization

public interface ISerialize
{
    read:  func(self, IDecoder) throw
    write: func(self, IEncoder)
}

public struct(T, READ: bool) Serializer
{
    ser: T
}

public impl Serializer
{
    mtd(MT) serialize(name: string, res: *MT) throw
    {
    #if READ
        ser.read(name, res, @typeof(MT))
    #else
        ser.write(name, @typeof(MT), res)
    }

    mtd serializeS32(name: string, res: *s32) throw
    {
    #if READ
        dref res = ser.readS32(name)
    #else
        ser.writeS32(name, dref res)
    }
}