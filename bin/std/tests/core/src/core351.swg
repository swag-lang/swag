using Core, Serialization

struct Toto
{
    arr: Array'String
}

#test
{
    var buf: ConcatBuffer
    var wri: Encoder'Write.JSon

    var toto: Toto
    toto.arr.add(String.from("toto"))
    toto.arr.add(String.from("titi"))
    toto.arr.add(String.from("tata"))

    wri.writeAll(&buf, toto)

    str := buf.toString()
    //Console.print(str, "\n")

    var rea: Decoder'Read.JSon
    v := try rea.readAll'Toto(str.toSlice())
    defer Memory.delete(v)

    @assert(v.arr.count == 3)
    @assert(dref v.arr[0] == "toto")
    @assert(dref v.arr[1] == "titi")
    @assert(dref v.arr[2] == "tata")

    //Console.print(dref v, "\n")
}