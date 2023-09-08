using Core, Serialization, Math

struct Toto1
{
    x: [10] f32
}

struct Toto2
{
    x: Array'f32
}

//#[Swag.PrintBc]
#test
{
    var enc: Encoder'Write.TagBin

    var x: Toto1
    var buf: ConcatBuffer
    x.x[9] = 9
    try enc.writeAll(&buf, x)
    buf.makeLinear()

    var dec: Decoder'Read.TagBin
    let x1 = try dec.readAll'Toto2(buf.toSlice())
    defer Memory.delete(x1)

    @assert(x1.x.count == @countof(x.x))
    @assert(x1.x[9] == 9)
}