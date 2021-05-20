using core, StrConv

#test
{
    {
        var cb: ConcatBuffer
        (v, ?) := try atod("0.00001")
        dtoa(&cb, v, "f"'u8, 5, false)
        str := cb.toString()
        //Console.print("result: %\n", str)
    }

    {
        var cb: ConcatBuffer
        (v, ?) := try atod("3.1414997518839567e-05")
        dtoa(&cb, v, "e"'u8, -1, false)
        str := cb.toString()
        //Console.print("result: %\n", str)
    }
}