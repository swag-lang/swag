using Core, Core.Format

#test
{
    var buf: ConcatBuffer
    Format.append(&buf, "titi%%")
    var str = buf.toString()
    //@print(cast(string) str)
    @assert(str == "titi%")
}

#test
{
    var buf: ConcatBuffer
    Format.append(&buf, "titi %", 666)
    var str = buf.toString()
    //@print(cast(string) str)
    @assert(str == "titi 666")
}