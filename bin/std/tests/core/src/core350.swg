using Core

struct Toto
{
    x, y:   bool
    z:      f32 = 3.14
    arr:    [10] s32
}

var toto: Toto

#test
{
    var buf: ConcatBuffer
    var ser: Serialization.Write.JSon
    var wri: Serialization.Encoder

    wri.init(&buf, ser)
    wri.write(toto)

    str := buf.toString()
    Console.print(str)
}