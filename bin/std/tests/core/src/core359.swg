using Core, Serialization

struct Toto
{
    x, y, z: f32
}

//#[Swag.PrintBc]
#test
{
    var enc: Encoder'Write.TagBin

    var x: Toto
    x.x = 1
    x.y = 2
    x.z = 3

    var buf: ConcatBuffer
    enc.write(&buf, x)
    buf.makeLinear()

    var dec: Decoder'Read.TagBin
    x1 := try dec.read'Toto(buf.toSlice())
    defer Memory.delete(x1)

    Console.print(dref x1)
}