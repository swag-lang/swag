using Core, Core.Serialization, Core.Math

struct Toto1
{
    x: f32
}

struct Toto2
{
    x: [4] f32
}

//#[Swag.PrintBc]
#test
{
    var enc: Encoder'Write.TagBin

    var x:   Toto1
    var buf: ConcatBuffer
    x.x = 1.5
    try enc.writeAll(&buf, x)
    buf.makeLinear()

    var dec: Decoder'Read.TagBin
    let x1   = try dec.readAll'Toto2(buf.toSlice())
    defer Memory.delete(x1)

    @assert(x1.x[0] == 1.5)
}
