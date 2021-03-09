
using core

#test
{
    var buf: ConcatBuffer
    format.append(&buf, "toto")
    str := buf.toString()
    @assert(str == "toto")
    //@print(cast(string) str)

    /*format(&buf, "titi##")
    str = buf.toString()
    @assert(str == "titi#")*/
}