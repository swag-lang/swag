
using Core

#test
{
    var buf: ConcatBuffer
    Format.append(&buf, "%", -3)
    var str = buf.toString()
    //@print(cast(string) str)
    @assert(str == "-3")
}
