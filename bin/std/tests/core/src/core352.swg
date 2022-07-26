using Core, Serialization

enum ARGB { A; R; G; B; }

struct Toto
{
    e: ARGB = A
}

#test
{
    var buf: Array'u8
    var wri: Encoder'Write.JSon

    var toto: Toto
    toto.e = .R

    wri.writeAll(&buf, toto)

    var rea: Decoder'Read.JSon
    v := try rea.readAll'Toto(buf.toSlice())
    defer Memory.delete(v)

    @assert(v.e == .R)
}