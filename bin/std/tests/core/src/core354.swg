using Core, Core.Serialization

struct V2
{
    x, y: f32
}

struct Toto
{
    v2: Array'V2
}

#test
{
    var buf: ConcatBuffer
    var wri: Encoder'Write.JSon

    var toto: Toto
    toto.v2.add({1, 2})
    toto.v2.add({3, 4})

    try wri.writeAll(&buf, toto)

    var str = buf.toString()
    //Console.print(str, "\n")

    var rea: Decoder'Read.JSon
    let v    = try rea.readAll'Toto(str.toSlice())
    defer Memory.delete(v)

    @assert(v.v2.count == 2)
    @assert(v.v2[0].x == 1)
    @assert(v.v2[0].y == 2)
    @assert(v.v2[1].x == 3)
    @assert(v.v2[1].y == 4)

    //Console.print(dref v, "\n")
}
