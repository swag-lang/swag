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

    wri.init(&buf)
    var toto: Toto
    toto.x = true
    toto.z = 3.14
    toto.arr[5] = 666

    wri.write(toto)

    str := buf.toString()
    Console.print(str)

    var rea: Decoder'Read.JSon
    v := cast(*Toto) try rea.read(str.toSlice(), Toto)
    Console.print(dref v)
    Memory.delete(v)
}