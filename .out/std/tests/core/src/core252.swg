using core, strconv

#test
{
    {
        var cb: ConcatBuffer
        (v, ?) := try atod("0.00001")
        dtoa(&cb, v, "f"'u8, 5)
        str := cb.toString()
        console.print("result: %\n", str)
    }

    {
        var cb: ConcatBuffer
        (v, ?) := try atod("3.1414997518839567e-05")
        dtoa(&cb, v, "e"'u8, -1)
        str := cb.toString()
        console.print("result: %\n", str)
    }
}