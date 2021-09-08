using Core

#test
{
    {
        var r: Parser.RegExp
        assume r.compile("abc")
        res := r.grep("ggggabc")
        @assert(res == "abc")
    }

    {
        var r: Parser.RegExp
        assume r.compile("a*")
        res := r.grep("XXXaaaaXXX")
        @assert(res == "aaaa")
    }
}