#import "std"
using std.text, std.collections

#test
{
    var buf: ConcatBuffer
    var fmt: ConvertFormatFloat
    fmt.value = -3.1415
    format(&buf, "%", fmt)
    str := buf.toString()
    @assert(str == "-3.14150")
}
