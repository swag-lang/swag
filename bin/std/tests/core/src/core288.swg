using Core

#test
{
    {
        var r: Parser.RegExp
        assume r.compile("(ab)*c")
        @assert(r.match("c"))
        @assert(r.match("abc"))
        @assert(r.match("ababc"))
        @assert(!r.match("abac"))
    }
}