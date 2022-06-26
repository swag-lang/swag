using Core

struct Toto
{
    x, y:   bool
    arr:    [10] bool
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