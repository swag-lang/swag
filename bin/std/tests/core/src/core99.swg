
using Core

#test
{
    using Core, Core.StrConv
    var buf: ConcatBuffer

    convertInt(&buf, 666's32, "X08")
    @assert(buf.toString() == "0000029A")

    buf.clear()
    convertInt(&buf, 666's32, "D-8")

    //@print(cast(string) buf.toString())
    @assert(buf.toString() == "-----666")
}
