using Core

#test
{
    {
        var r: Parser.RegExp
        assume r.compile("a{4}")
        @assert(r.match("aaaa"))
        @assert(!r.match("a"))
        @assert(!r.match("aaaaa"))
    }

    {
        var r: Parser.RegExp
        assume r.compile("a{4,}")
        @assert(r.match("aaaa"))
        @assert(r.match("aaaaa"))
    }
}
