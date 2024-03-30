using Core

#test
{
    var buf: ConcatBuffer
    Format.append(&buf, "toto")

    var str = Format.toString("%", 1)
}
