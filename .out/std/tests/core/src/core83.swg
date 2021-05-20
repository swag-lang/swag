
using core

#test
{
    var buf: ConcatBuffer
    Format.append(&buf, "toto")

    str := Format.toString("%", 1)
}