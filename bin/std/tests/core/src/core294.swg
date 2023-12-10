using Core

#test
{
    {
        var r: Parser.RegExp
        assume r.compile("abc")
        let res = r.grepAll("abcggggabcggggabc")
        @assert(res.count == 3)
    }

    {
        var r: Parser.RegExp
        assume r.compile("a+")
        let res = r.grepAll("abcggggaaaa")
        @assert(res.count == 2)
        @assert(res[0].value == "a")
        @assert(res[1].value == "aaaa")
    }

    {
        var r: Parser.RegExp
        assume r.compile("A+", ignoreCase: true)
        let res = r.grepAll("abcggggaaaa")
        @assert(res.count == 2)
        @assert(res[0].value == "a")
        @assert(res[1].value == "aaaa")
    }
}