#import "core"
using core

#test
{
    {
        res := unicode32.fromUtf8("abc")
        unicode32.makeUpper(res.toSlice())
        @assert(res[0] == "A"'char)
        @assert(res[1] == "B"'char)
        @assert(res[2] == "C"'char)
        //console.print("%", res)
    }

    {
        res := unicode32.fromUtf8("éxàmple")
        unicode32.makeUpper(res.toSlice())
        //console.print("%", res)
        @assert(res[0] == "É"'char)
        @assert(res[2] == "À"'char)
    }
}