#global export
#global namespace Serialization

interface ISerialize
{
    read:           mtd(u32, IDecoder)->bool throw
    write:          mtd(u32, IEncoder)->bool
    readElement:    mtd(u32, string, typeinfo, *void, IDecoder)->bool throw
    writeElement:   mtd(u32, string, typeinfo, const *void, IEncoder)->bool
}

struct(T) Serializer
{
    #assert @typeof(T) == IDecoder or @typeof(T) == IEncoder
    ser: T
}

impl Serializer
{
    mtd(MT) serialize(name: string, res: *MT) throw
    {
        #if T == IDecoder
        {
            if !ser.beginElement(name, MT)
                return
            ser.beginValue()

            #if MT == s32
                dref res = ser.readS32()
            #else
                ser.read(res, @typeof(MT))

            ser.endValue()
            ser.endElement()
        }
        #else
        {
            ser.beginElement(name, MT)
            ser.beginValue()

            #if MT == s32
                ser.writeS32(dref res)
            #else
                ser.write(@typeof(MT), res)

            ser.endValue()
            ser.endElement()
        }
    }

    mtd serializeBufferU8(name: string, res: **u8, count: uint) throw
    {
        #if T == IDecoder
        {
            if !ser.beginElement(name, *u8)
                return
            ser.beginValue()
            dref res = ser.readBufferU8(count)
            ser.endValue()
            ser.endElement()
        }
        #else
        {
            ser.beginElement(name, *u8)
            ser.beginValue()
            ser.writeBufferU8(dref res, count)
            ser.endValue()
            ser.endElement()
        }
    }
}