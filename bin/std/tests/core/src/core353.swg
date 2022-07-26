using Core, Serialization

#[NoSerialize]
struct V2 { x, y: f32; }

struct Toto
{
    v2: V2
}

#test
{
    var buf: Array'u8
    var wri: Encoder'Write.JSon

    var toto: Toto
    toto.v2.x = 666

    wri.writeAll(&buf, toto)

    var rea: Decoder'Read.JSon
    v := try rea.readAll'Toto(buf.toSlice())
    defer Memory.delete(v)

    @assert(v.v2.x == 0)
}