using Core, Serialization

struct Toto
{
    x, y:   bool
    z:      f32
    arr:    [10] s32
}

#test
{
    var buf: ConcatBuffer
    var wri: Encoder'Write.JSon

    var toto: Toto
    toto.x = true
    toto.z = 3.14
    toto.arr[5] = 666

    wri.write(&buf, toto)

    str := buf.toString()
    Console.print(str)

    var rea: Decoder'Read.JSon
    v := try rea.read'Toto(str.toSlice())
    defer Memory.delete(v)

    Console.print(dref v)
}