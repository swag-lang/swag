using Core, Serialization, Math

struct Toto1
{
    x: Array'Vector4
}

//#[Swag.PrintBc]
#test
{
    //Time.Stopwatch.scopeMeasure()
    var enc: Encoder'Write.TagBin

    var x: Toto1
    loop 1000 do
        x.x.add({1, 2, 3, 4})
    var buf: ConcatBuffer
    try enc.writeAll(&buf, x)
    buf.makeLinear()

    var dec: Decoder'Read.TagBin
    let x1 = try dec.readAll'Toto1(buf.toSlice())
    defer Memory.delete(x1)

    @assert(x1.x.count == 1000)
    @assert(x1.x[0].x == 1)
    @assert(x1.x[0].y == 2)
    @assert(x1.x[0].z == 3)
    @assert(x1.x[0].w == 4)
}