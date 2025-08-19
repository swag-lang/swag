using Core, Core.Serialization, Core.Math

struct Toto1
{
    x:   [10] f32
}

struct Toto2
{
    x:   Array'f32
}

//#[Swag.PrintBc]
#test
{
    var enc: Encoder'Write.TagBin

    var x:   Toto2
    var buf: ConcatBuffer
    x.x.add(1)
    x.x.add(9)
    try enc.writeAll(&buf, x)
    buf.makeLinear()

    var dec: Decoder'Read.TagBin
    let x1   = try dec.readAll'Toto1(buf.toSlice())
    defer Memory.delete(x1)

    @assert(x1.x[0] == 1)
    @assert(x1.x[1] == 9)
}
