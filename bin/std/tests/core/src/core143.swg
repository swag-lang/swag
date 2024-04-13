using Core

#test
{
    var x = [[1, 2], [3, 4]]

    var buf: ConcatBuffer
    StrConv.convertAny(&buf, x, null)

    var str = buf.toString()
    str.replace(" ", "")
    //Console.print(str)
    //Console.print("XX ", str)
    @assert(str == "[[1,2],[3,4]]")
}
