using Core, Core.Format

#test
{
    var buf: ConcatBuffer
    Format.append(&buf, "titi%%")
    let str = buf.toString()
    //@print(cast(string) str)
    @assert(str == "titi%")
}

#test
{
    var buf: ConcatBuffer
    Format.append(&buf, "titi %", 666)
    let str = buf.toString()
    //@print(cast(string) str)
    @assert(str == "titi 666")
}
