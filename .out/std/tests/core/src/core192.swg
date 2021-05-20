
using Core

#test
{
    {
        res := Unicode.fromUtf8("abc")
        Unicode.makeUpper(res.toSlice())
        @assert(res[0] == "A"'rune)
        @assert(res[1] == "B"'rune)
        @assert(res[2] == "C"'rune)
        //Console.print("%", res)
    }

    {
        res := Unicode.fromUtf8("éxàmple")
        Unicode.makeUpper(res.toSlice())
        //Console.print("%", res)
        @assert(res[0] == "É"'rune)
        @assert(res[2] == "À"'rune)
    }
}