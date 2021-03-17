
using core

#test
{
    var buf: ConcatBuffer
    format.append(&buf, "toto")

    str := format.toString("%", 1)
}