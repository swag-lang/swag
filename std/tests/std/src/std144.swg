#import "std"
using std

#test
{
    {
        var x: Array's32 = @[1,2,3,4]

        var buf: ConcatBuffer
        strconv.convertAny(&buf, x, null)

        str := buf.toString()
        //console.print("%\n", str)
        @assert(str == "[1,2,3,4]")
    }

    {
        var x: Array'string = @["a","b","c","d"]
        str := format.toString("%", x)
        //console.print("%\n", str)
        @assert(str == "[a,b,c,d]")
    }

    {
        var x: Array'String
        x.addAny("toto")
        x.addAny("titi")
        str := format.toString("arr=%", x)
        //console.print("%\n", str)
        @assert(str == "arr=[toto,titi]")
    }
}
