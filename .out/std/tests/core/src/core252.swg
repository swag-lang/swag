using core, strconv

#test
{
    var cb: ConcatBuffer
    ftoa64(&cb, 1.0e-5, "f"'u8, 5)
    str := cb.toString()
    console.print("result: %\n", str)
}