using Core, StrConv

#test
{
    {
        var cb: ConcatBuffer
        var (v, ?, ?) = try Atod.parse("0.00001")
        Dtoa.parse(&cb, v, `f`'u8, 5, false)
        var str = cb.toString()
        //Console.print("result: %\n", str)
    }

    {
        var cb: ConcatBuffer
        var (v, ?, ?) = try Atod.parse("3.1414997518839567e-05")
        Dtoa.parse(&cb, v, `e`'u8, -1, false)
        var str = cb.toString()
        //Console.print("result: %\n", str)
    }
}