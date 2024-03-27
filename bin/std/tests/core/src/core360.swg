using Core, Core.Serialization

struct Toto1
{
    x, y, z: s32
}

struct Toto2
{
    str:     String
    z, y, x: s32
}

//#[Swag.PrintBc]
#test
{
    var enc: Encoder'Write.TagBin

    var x: Toto1
    x.x = 1
    x.y = 2
    x.z = 3

    var buf: ConcatBuffer
    try enc.writeAll(&buf, x)
    buf.makeLinear()

    var dec: Decoder'Read.TagBin
    let x1   = try dec.readAll'Toto2(buf.toSlice())
    defer Memory.delete(x1)

    //Console.print(dref x1)
    @assert(x1.x == 1)
    @assert(x1.y == 2)
    @assert(x1.z == 3)
}
