using core, strconv

#test
{
    {
        var cb: ConcatBuffer
        (v, ?) := try atof64("0.00001")
        ftoa64(&cb, v, "f"'u8, 5)
        str := cb.toString()
        console.print("result: %\n", str)
    }

    {
        var cb: ConcatBuffer
        (v, ?) := try atof64("3.1414997518839567e-05")
        ftoa64(&cb, v, "e"'u8, -1)
        str := cb.toString()
        console.print("result: %\n", str)
    }
}