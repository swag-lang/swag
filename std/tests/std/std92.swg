#import "std"
using std.text, std.collections

#test
{
    var buf: ConcatBuffer
    //format(&buf, "value: %", ConvertFormatFloat(-3.1415))
    convertAny(&buf, ConvertFormatFloat(-3.1415))
    str := buf.toString()
    //@print(cast(string) str)
    @assert(str == "-3.14150")
}
