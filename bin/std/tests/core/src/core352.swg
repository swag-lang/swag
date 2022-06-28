using Core, Serialization

enum ARGB { A; R; G; B; }

struct Toto
{
    e: ARGB = A
}

#test
{
    var buf: ConcatBuffer
    var wri: Encoder'Write.JSon

    var toto: Toto
    toto.e = .R

    wri.write(&buf, toto)

    str := buf.toString()
    //Console.print(str, "\n")

    var rea: Decoder'Read.JSon
    v := try rea.read'Toto(str.toSlice())
    defer Memory.delete(v)

    @assert(v.e == .R)

    //Console.print(dref v, "\n")
}