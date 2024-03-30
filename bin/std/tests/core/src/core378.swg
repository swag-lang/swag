using Core, Core.Serialization, Core.Math

struct Toto1
{
    x: Array'Vector3
}

struct Toto2
{
    x: Vector3
}

//#[Swag.PrintBc]
#test
{
    var enc: Encoder'Write.TagBin

    var x:   Toto1
    var buf: ConcatBuffer
    x.x.add({0, 0, 1.5})
    try enc.writeAll(&buf, x)
    buf.makeLinear()

    var dec: Decoder'Read.TagBin
    let x1   = try dec.readAll'Toto2(buf.toSlice())
    defer Memory.delete(x1)

    @assert(x1.x.z == 1.5)
}
