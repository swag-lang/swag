using Core, Core.StrConv

#test
{
    var buf: ConcatBuffer
    convertInt(&buf, 123's32, {}, null)
}

#test
{
    var buf: ConcatBuffer
    convertInt(&buf, 123456's32, {}, null)
    var str = buf.toString()
    @assert(str == "123456")
}

#test
{
    var buf: ConcatBuffer
    var fmt: ConvertFormatInt

    fmt.base = 16
    convertInt(&buf, 0xFFFFFFFF, fmt)
    var str = buf.toString()
    @assert(str == "FFFFFFFF")
    //Console.write(cast(string) str)
}
