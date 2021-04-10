using core, strconv

#test
{
    var cb: ConcatBuffer
    f64ToA(&cb, 3.14, "f"'u8, 5)
    str := cb.toString()
    console.print("result: %\n", str)
}