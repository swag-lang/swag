using Core

#test
{
    var buf: ConcatBuffer
    Format.append(&buf, "toto")
    var str = buf.toString()
    @assert(str == "toto")
}
