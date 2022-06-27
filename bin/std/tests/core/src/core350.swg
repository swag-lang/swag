using Core, Serialization

struct Toto
{
    x, y:   bool
    z:      f32
    arr:    [10] s32
    str:    String
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

    wri.write(&buf, toto)

    str := buf.toString()
    //Console.print(str, "\n")

    var rea: Decoder'Read.JSon
    v := try rea.read'Toto(str.toSlice())
    defer Memory.delete(v)

    //Console.print(dref v, "\n")
}