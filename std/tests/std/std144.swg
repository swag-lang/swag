#import "std"
using std

#test
{
    var x: Array's32 = @[1,2,3,4]

    var buf: ConcatBuffer
    strconv.convertAny(&buf, x, null)

    str := buf.toString()
    console.print("%\n", str)
    //@assert(str == "[[1,2],[3,4]]")
}
