using Core, Core.StrConv
/*
#test
{
    var buf: ConcatBuffer
    //format(&buf, "value: %", ConvertFormatFloat(-3.1415))
    var fmt: ConvertFormatFloat
    fmt.value     = -3.1
    fmt.precision = 1
    convertAny(&buf, fmt, null)
    var str = buf.toString()
    @assert(str == "-3.1")
}
*/
