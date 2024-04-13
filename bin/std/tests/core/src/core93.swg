using Core, Core.StrConv
/*
#test
{
    var buf: ConcatBuffer
    var fmt: ConvertFormatFloat
    fmt.value     = -3.1415
    fmt.precision = 5
    Format.append(&buf, "%", fmt)
    var str = buf.toString()
    @assert(str == "-3.14150")
}
*/