using Core, Core.Serialization

struct Toto
{
    a: typeinfo
}

#test
{
    var buf: ConcatBuffer
    var wri: Encoder'Write.JSon

    var toto: Toto
    toto.a = u32

    try wri.writeAll(&buf, toto)

    var str = buf.toString()
    //Console.print(str, "\n")

    var rea: Decoder'Read.JSon
    let v    = try rea.readAll'Toto(str.toSlice())
    defer Memory.delete(v)

    @assert(v.a == u32)
}
