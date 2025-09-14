using Core

#test
{
    var buf: ConcatBuffer
    Format.append(&buf, "toto")
    let str = buf.toString()
    @assert(str == "toto")
    //@print(cast(string) str)

    /*format(&buf, "titi##")
    str = buf.toString()
    @assert(str == "titi#")*/
}
