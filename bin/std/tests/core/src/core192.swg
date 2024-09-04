using Core

#test
{
    {
        var res = Unicode.fromUtf8("abc")
        Unicode.makeUpper(res.toSlice())
        @assert(res[0] == 'A')
        @assert(res[1] == 'B')
        @assert(res[2] == 'C')
        //Console.print("%", res)
    }

    {
        var res = Unicode.fromUtf8("éxàmple")
        Unicode.makeUpper(res.toSlice())
        //Console.print("%", res)
        @assert(res[0] == 'É')
        @assert(res[2] == 'À')
    }
}
