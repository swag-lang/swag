#global skip
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
    var buf: ConcatBuffer
    var wri: Encoder'Write.JSon

    var toto: Toto

    toto.a.alloc(Titi)
    toto.a.value.x = 1
    toto.a.value.y = 2
    toto.a.value.z = 3

    try wri.writeAll(&buf, toto)

    var str = buf.toString()
    //Console.print(str, "\n")

    var rea: Decoder'Read.JSon
    var v = try rea.readAll'Toto(str.toSlice())
    defer Memory.delete(v)

    @assert(v.a.value.x == 1)
    @assert(v.a.value.y == 2)
    @assert(v.a.value.z == 3)

    //Console.print(dref v, "\n")
}