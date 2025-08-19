using Core, Core.Serialization

enum ARGB
{
    A
    R
    G
    B
}

struct Toto
{
    e:   ARGB = A
}

#test
{
    var buf: ConcatBuffer
    var wri: Encoder'Write.JSon

    var toto: Toto
    toto.e = .R

    try wri.writeAll(&buf, toto)

    var str = buf.toString()
    //Console.print(str, "\n")

    var rea: Decoder'Read.JSon
    let v    = try rea.readAll'Toto(str.toSlice())
    defer Memory.delete(v)

    @assert(v.e == .R)

    //Console.print(dref v, "\n")
}
