using Core

#test
{
    {
        var x: Array's32 = [1, 2, 3, 4]

        var buf: ConcatBuffer
        StrConv.convertAny(&buf, x, {}, null)

        var str = buf.toString()
        //Console.print("%\n", str)
        str.replace(" ", "")
        @assert(str == "[1,2,3,4]")
    }

    {
        var x:  Array'string = ["a", "b", "c", "d"]
        var str = Format.toString("%", x)
        //Console.print("%\n", str)
        str.replace(" ", "")
        @assert(str == "[a,b,c,d]")
    }

    {
        var x: Array'String
        x.add("toto")
        x.add("titi")
        var str = Format.toString("arr=%", x)
        //Console.print("%\n", str)
        str.replace(" ", "")
        @assert(str == "arr=[toto,titi]")
    }
}
