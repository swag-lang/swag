
using Core

#test
{
    var buf: ConcatBuffer
    Format.append(&buf, "% 汉 % %", 1, 2, 3)
    str := buf.toString()
    //@print(cast(string) str)
    @assert(str == "1 汉 2 3")
}
