
using Core, Core.StrConv

#test
{
    var buf: ConcatBuffer
    convertInt(&buf, 123's32, null)
}

#test
{
    var buf: ConcatBuffer
    convertInt(&buf, 123456's32, null)
    str := buf.toString()
    @assert(str == "123456")
}

#test
{
    var buf: ConcatBuffer
    var fmt: ConvertFormatInt

    fmt.value = 0xFFFFFFFF
    fmt.base = 16
    convertInt(&buf, fmt)
    str := buf.toString()
    @assert(str == "FFFFFFFF")
    //Console.write(cast(string) str)
}