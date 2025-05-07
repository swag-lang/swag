using Core, Core.Serialization, Core.Math

#[Version(1)]
struct Toto1
{
    x: f32
}

#[Version(2)]
struct Toto2
{
    x: f32
}

//#[Swag.PrintBc]
#test
{
    var enc: Encoder'Write.TagBin

    var x: Toto2
    x.x = 666
    var buf: ConcatBuffer
    try enc.writeAll(&buf, x)
    buf.makeLinear()

    var dec: Decoder'Read.TagBin
    var x1   = catch dec.readAll'Toto1(buf.toSlice())
    @assert(@err == Core.Errors.InvalidVersion)
}
