using Core, Core.Serialization, Core.Math

struct Toto1
{
    x: [13] s32
}

struct Toto2
{
    x: [2] s32
}

//#[Swag.PrintBc]
#test
{
    var enc: Encoder'Write.TagBin

    var x: Toto1
    x.x[0] = 1
    x.x[1] = 2
    var buf: ConcatBuffer
    try enc.writeAll(&buf, x)
    buf.makeLinear()

    var dec: Decoder'Read.TagBin
    let x1   = try dec.readAll'Toto2(buf.toSlice())
    defer Memory.delete(x1)

    @assert(x1.x[0] == 1)
    @assert(x1.x[1] == 2)
}
