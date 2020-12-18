#import "core"
using core, core.format

#test
{
    var buf: ConcatBuffer
    format.append(&buf, "titi%%")
    str := buf.toString()
    //@print(cast(string) str)
    @assert(str == "titi%")
}

#test
{
    var buf: ConcatBuffer
    format.append(&buf, "titi %", 666)
    str := buf.toString()
    //@print(cast(string) str)
    @assert(str == "titi 666")
}