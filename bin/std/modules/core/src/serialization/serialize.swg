#global namespace Serialization

public interface ISerialize
{
    read:  func(self, IDecoder) throw
    write: func(self, IEncoder)
}

public struct(T, READ: bool) Serializer
{
    ser: *T
}

public impl Serializer
{
    mtd serializeS32(res: *s32, name: string) throw
    {
        ser.beginElement(name, s32)
        ser.beginValue(s32)

    #if READ
        dref res = ser.readS32()
    #else
        ser.writeS32(dref res)

        ser.endElement()
        ser.endValue()
    }
}