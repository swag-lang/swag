
using core, core.strconv

#test
{
    var buf: ConcatBuffer
    //format(&buf, "value: %", ConvertFormatFloat(-3.1415))
    var fmt: ConvertFormatFloat
    fmt.value = -3.1415
    convertAny(&buf, fmt, null)
    str := buf.toString()
    @assert(str == "-3.14150")
}
