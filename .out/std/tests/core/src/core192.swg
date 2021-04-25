
using core

#test
{
    {
        res := unicode.fromUtf8("abc")
        unicode.makeUpper(res.toSlice())
        @assert(res[0] == "A"'rune)
        @assert(res[1] == "B"'rune)
        @assert(res[2] == "C"'rune)
        //console.print("%", res)
    }

    {
        res := unicode.fromUtf8("éxàmple")
        unicode.makeUpper(res.toSlice())
        //console.print("%", res)
        @assert(res[0] == "É"'rune)
        @assert(res[2] == "À"'rune)
    }
}