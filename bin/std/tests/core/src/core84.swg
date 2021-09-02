
using Core, Core.StrConv

#test
{
    var buf: ConcatBuffer
    //@print("starts\n")
    convertAny(&buf, 666, null)
    str := buf.toString()
    @assert(str == "666")
    //@print(cast(string) str)
}