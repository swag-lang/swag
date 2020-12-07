#import "std"
using std

#test
{
    var buf: ConcatBuffer
    format.append(&buf, "%", -3)
    str := buf.toString()
    //@print(cast(string) str)
    @assert(str == "-3")
}
