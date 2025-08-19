using Core, Core.Serialization

struct AA
{
    #[Compress]
    bb:  BB
}

#[Alias("AA"), Final]
struct BB { x, y, z: s32 }

//#[Swag.PrintBc]
#test
{
    var enc: Encoder'Write.TagBin

    var x:   AA
    var buf: ConcatBuffer
    x.bb.x = 1
    x.bb.y = 2
    x.bb.z = 3
    try enc.writeAll(&buf, x)
    buf.makeLinear()

    var dec: Decoder'Read.TagBin
    let x1   = try dec.readAll'AA(buf.toSlice())
    defer Memory.delete(x1)

    @assert(x1.bb.x == 1)
    @assert(x1.bb.y == 2)
    @assert(x1.bb.z == 3)
}
