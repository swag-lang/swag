using Core, Core.Serialization

#[NoSerialize]
struct V2 { x, y: f32; }

struct Toto
{
    v2: V2
}

#test
{
    var buf: ConcatBuffer
    var wri: Encoder'Write.JSon

    var toto: Toto
    toto.v2.x = 666

    try wri.writeAll(&buf, toto)

    var str = buf.toString()
    //Console.print(str, "\n")

    var rea: Decoder'Read.JSon
    let v = try rea.readAll'Toto(str.toSlice())
    defer Memory.delete(v)

    @assert(v.v2.x == 0)

    //Console.print(dref v, "\n")
}