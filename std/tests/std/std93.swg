#import "std"
using std.collections, std.strconv

#test
{
    var buf: ConcatBuffer
    var fmt: ConvertFormatFloat
    fmt.value = -3.1415
    format.append(&buf, "%", fmt)
    str := buf.toString()
    @assert(str == "-3.14150")
}
