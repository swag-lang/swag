
using core

#test
{
    var buf: ConcatBuffer
    Format.append(&buf, "value: %{5}", -3.1415)
    str := buf.toString()
    //@print(cast(string) str)
    @assert(str == "value: -3.14150")
}
