
using core

#test
{
    var buf: ConcatBuffer
    Format.append(&buf, "%: %", "toto", 3'u32)
    str := buf.toString()
    //@print(cast(string) str)
    @assert(str == "toto: 3")
}
