using Core, Core.Serialization, Core.Math

struct Toto1
{
    x: s32
    y: f64
}

var toto1: Toto1
var toto2: Toto1

//#[Swag.PrintBc]
#test
{
    var enc: Encoder'Write.TagBin
    var dec: Decoder'Read.TagBin

    var buf: ConcatBuffer

    //#[Swag.PrintBc]
    func readWrite(where: *Toto1, ser: *Serializer) throw
    {
        ser.beginSection(.Unordered)
        ser.serialize("x", &where.x)
        ser.serialize("y", &where.y)
        ser.endSection()
    }

    toto1.x = 666
    toto1.y = 665

    try
    {
        var ser: Serializer
        ser.startWrite(enc, &buf)
        readWrite(&toto1, &ser)
        ser.end()

        buf.makeLinear()
        ser.startRead(dec, buf.toSlice())
        readWrite(&toto2, &ser)
        ser.end()

        @assert(Memory.equals(toto1, toto2))
    }
}