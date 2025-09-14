using Core

#test
{
    var buf: ConcatBuffer
    Format.append(&buf, "%", -3)
    let str = buf.toString()
    //@print(cast(string) str)
    @assert(str == "-3")
}
