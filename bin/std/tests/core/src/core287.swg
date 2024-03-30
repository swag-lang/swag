using Core

#test
{
    {
        var r: Parser.RegExp
        assume r.compile("ab")
        @assert(r.match("ab"))
    }

    {
        var r: Parser.RegExp
        assume r.compile("a*")
        @assert(r.match(""))
        @assert(r.match("a"))
        @assert(r.match("aa"))
        @assert(r.match("aaa"))
        @assert(!r.match("b"))
        @assert(!r.match("aab"))
    }

    {
        var r: Parser.RegExp
        assume r.compile("a*b")
        @assert(r.match("b"))
    }

    {
        var r: Parser.RegExp
        assume r.compile("a*b")
        @assert(!r.match("a"))
    }

    {
        var r: Parser.RegExp
        assume r.compile("a+b")
        @assert(r.match("ab"))
        @assert(r.match("aab"))
        @assert(!r.match("b"))
    }

    {
        var r: Parser.RegExp
        assume r.compile("fe?d")
        @assert(r.match("fed"))
        @assert(r.match("fd"))
        @assert(!r.match("feed"))
    }
}
