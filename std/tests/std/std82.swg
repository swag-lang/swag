#import "std"
using std.text, std.collections

#test
{
    var buf: ConcatBuffer
    convertInt(&buf, 123's32)
}

#test
{
    var buf: ConcatBuffer
    convertInt(&buf, 123456's32)
    str := buf.toString()
    @assert(str == "123456")
}

#test
{
    var buf: ConcatBuffer
    var fmt: ConvertFormatInt

    fmt.unsignedValue = 0xFFFFFFFF
    fmt.signed = false
    fmt.base = 16
    fmt.bits = 32
    convertInt(&buf, fmt)
    str := buf.toString()
    @assert(str == "FFFFFFFF")
    //console.write(cast(string) str)
}