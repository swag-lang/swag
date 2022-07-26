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

    var buf: Array'u8
    enc.writeAll(&buf, x)

    var dec: Decoder'Read.TagBin
    x1 := try dec.readAll'Toto(buf.toSlice())
    defer Memory.delete(x1)

    @assert(x1.x == 1)
    @assert(x1.y == 2)
    @assert(x1.z == 3)
}