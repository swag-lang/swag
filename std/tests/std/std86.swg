#import "std"
using std.text, std.collections

#test
{
    var buf: ConcatBuffer
    format(&buf, "titi%%")
    str := buf.toString()
    //@print(cast(string) str)
    @assert(str == "titi%")
}

#test
{
    var buf: ConcatBuffer
    format(&buf, "titi %", 666)
    str := buf.toString()
    //@print(cast(string) str)
    @assert(str == "titi 666")
}