
using Core

#test
{
    var x = [[1, 2], [3, 4]]

    var buf: ConcatBuffer
    StrConv.convertAny(&buf, x, null)

    var str = buf.toString()
    @assert(str == "[[1,2],[3,4]]")
}
