using Core, Core.Serialization

#[Final]
struct AA
{
    x, y, z: f32
}

struct Toto1
{
    x: AA
}

#[Alias("AA")]
struct BB
{
    x, y, z: f32
}

struct Toto2
{
    x: BB
}

//#[Swag.PrintBc]
#test
{
    var enc: Encoder'Write.TagBin

    var x:   Toto1
    var buf: ConcatBuffer
    x.x.z = 1.5
    try enc.writeAll(&buf, x)
    buf.makeLinear()

    var dec: Decoder'Read.TagBin
    let x1   = catch dec.readAll'Toto2(buf.toSlice())
    defer Memory.delete(x1)

    @assert(@err != null)
}
