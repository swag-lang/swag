using Core, Serialization

struct Toto
{
    a: typeinfo
}

#test
{
    var buf: Array'u8
    var wri: Encoder'Write.JSon

    var toto: Toto
    toto.a = u32

    wri.writeAll(&buf, toto)

    var rea: Decoder'Read.JSon
    v := try rea.readAll'Toto(buf.toSlice())
    defer Memory.delete(v)

    @assert(v.a == u32)
}