using Core, Serialization, Math

//#[Swag.PrintBc]
#test
{
    var enc: Encoder'Write.TagBin

    var x: Point
    x.x = 1
    x.y = 2

    var buf: ConcatBuffer
    try enc.writeAll(&buf, x)
    buf.makeLinear()

    var dec: Decoder'Read.TagBin
    var x1 = try dec.readAll'Point(buf.toSlice())
    defer Memory.delete(x1)

    //Console.print(dref x1)
    @assert(x1.x == 1)
    @assert(x1.y == 2)
}