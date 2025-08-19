using Core, Core.Serialization, Core.Math

#[Version(1)]
struct Toto1
{
    x:   f32
}

//#[Swag.PrintBc]
#test
{
    var enc: Encoder'Write.TagBin

    var x: Toto1
    x.x = 666
    var buf: ConcatBuffer
    try enc.writeAll(&buf, x)
    buf.makeLinear()

    var dec: Decoder'Read.TagBin
    let x1   = try dec.readAll'Toto1(buf.toSlice())
    defer Memory.delete(x1)

    @assert(x1.x == 666)
}
