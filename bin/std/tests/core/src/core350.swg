using Core, Serialization

#[Version(4)]
struct Toto
{
    x, y:   bool
    z:      f32
    arr:    [10] s32
    str:    String
    arr2:   Array's32
}

#test
{
    var buf: ConcatBuffer
    var wri: Encoder'Write.JSon

    var toto: Toto
    toto.x = true
    toto.z = 3.14
    toto.arr[5] = 666
    toto.str = "my string"
    toto.arr2.add([1, 2, 3])

    try wri.writeAll(&buf, toto)

    var str = buf.toString()
    //Console.print(str, "\n")

    var rea: Decoder'Read.JSon

    let v = try rea.readAll'Toto(str.toSlice())
    defer Memory.delete(v)

    @assert(v.x == true)
    @assert(v.y == false)
    @assert(Math.isEqualEpsilon(v.z, 3.14))
    @assert(v.arr[5] == 666)
    @assert(v.str == "my string")
    @assert(v.arr2.count == 3)
    @assert(v.arr2[0] == 1)
    @assert(v.arr2[1] == 2)
    @assert(v.arr2[2] == 3)

    //Console.print(dref v, "\n")
}