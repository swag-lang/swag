#import "std"
using std

#test
{
    var buf: ConcatBuffer
    format.append(&buf, "%: %", "toto", 3'u32)
    str := buf.toString()
    //@print(cast(string) str)
    @assert(str == "toto: 3")
}
