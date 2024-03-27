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

    {
        var r: Parser.RegExp
        assume r.compile("a大*")
        @assert(r.match("a大"))
        @assert(r.match("a大大大"))
    }

    {
        var r: Parser.RegExp
        assume r.compile("a|b")
        @assert(r.match("a"))
        @assert(r.match("b"))
        @assert(!r.match("ab"))
    }

    {
        var r: Parser.RegExp
        assume r.compile(".b.")
        @assert(r.match("xbx"))
        @assert(r.match("abc"))
        @assert(!r.match("acc"))
    }

    {
        var r: Parser.RegExp
        assume r.compile("\\(")
        @assert(r.match("("))
        assume r.compile("\\)")
        @assert(r.match(")"))
    }
}
