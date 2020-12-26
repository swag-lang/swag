#import "core"
using core

#test
{
    x := @[[1, 2], [3, 4]]

    var buf: ConcatBuffer
    strconv.convertAny(&buf, x, null)

    str := buf.toString()
    @assert(str == "[[1,2],[3,4]]")
}
