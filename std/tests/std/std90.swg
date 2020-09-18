#import "std"
using std

#test
{
    var buf: ConcatBuffer
    format.append(&buf, "value: %", -3.1415)
    str := buf.toString()
    //@print(cast(string) str)
    @assert(str == "value: -3.14150")
}
