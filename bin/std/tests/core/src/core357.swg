using Core, Serialization, System

struct Titi
{
    x, y, z: f32
}

struct Titi0
{
    using base: Titi
}

struct Toto
{
    a: Object'Titi
}

#test
{
    var buf: Array'u8
    var wri: Encoder'Write.JSon

    var toto: Toto

    toto.a.alloc(Titi)
    toto.a.value.x = 1
    toto.a.value.y = 2
    toto.a.value.z = 3

    wri.writeAll(&buf, toto)

    var rea: Decoder'Read.JSon
    v := try rea.readAll'Toto(buf.toSlice())
    defer Memory.delete(v)

    @assert(v.a.value.x == 1)
    @assert(v.a.value.y == 2)
    @assert(v.a.value.z == 3)
}