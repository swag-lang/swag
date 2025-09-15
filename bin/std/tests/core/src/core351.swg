using Core, Core.Serialization

struct Toto
{
    arr: Array'String
}

#test
{
    var buf: ConcatBuffer
    var wri: Encoder'Write.JSon

    let toto: Toto
    toto.arr.add(String.from("toto"))
    toto.arr.add(String.from("titi"))
    toto.arr.add(String.from("tata"))

    try wri.writeAll(&buf, toto)

    var str = buf.toString()
    //Console.print(str, "\n")

    var rea: Decoder'Read.JSon
    let v    = try rea.readAll'Toto(str.toSlice())
    defer Memory.delete(v)

    @assert(v.arr.count == 3)
    @assert(v.arr[0] == "toto")
    @assert(v.arr[1] == "titi")
    @assert(v.arr[2] == "tata")

    //Console.print(dref v, "\n")
}
