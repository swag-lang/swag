
using Core

#test
{
    {
        var x: Array's32 = @[1,2,3,4]

        var buf: ConcatBuffer
        StrConv.convertAny(&buf, x, null)

        str := buf.toString()
        //Console.print("%\n", str)
        @assert(str == "[1,2,3,4]")
    }

    {
        var x: Array'string = @["a","b","c","d"]
        str := Format.toString("%", x)
        //Console.print("%\n", str)
        @assert(str == "[a,b,c,d]")
    }

    {
        var x: Array'String
        x.addAny("toto")
        x.addAny("titi")
        str := Format.toString("arr=%", x)
        //Console.print("%\n", str)
        @assert(str == "arr=[toto,titi]")
    }
}
