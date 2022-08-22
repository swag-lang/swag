using Core, Serialization

enum ARGB { A; R; G; B; }

struct Toto
{
    e: ARGB = A
}

#test
{
    var ser: Serializer

    try
    {
        var buf: ConcatBuffer
        var wri: Encoder'Write.JSon
        ser.startWrite(wri, &buf)
        ser.beginSection(.Ordered)
        x := 666's32
        ser.serialize("toto", &x)
        ser.endSection()
        ser.end()

        //str := buf.toString()
        //Console.print(str, "\n")
    }

    try
    {
        var rea: Decoder'Read.JSon
        ser.startRead(rea, buf.toSlice())
        ser.beginSection(.Ordered)
        y := 0's32
        ser.serialize("toto", &y)
        ser.endSection()
        ser.end()

        @assert(y == 666)
    }
}