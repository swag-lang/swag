using Core

#test
{
    {
        var r: Parser.RegExp
        assume r.compile("\\pL*")
        @assert(r.match("a"))
        @assert(r.match("目目目"))
    }

    {
        var r: Parser.RegExp
        assume r.compile("\\pN*")
        @assert(r.match("01234෦"))
        @assert(!r.match("a01234"))
    }

    {
        var r: Parser.RegExp
        assume r.compile("\\p{Ll}*")
        @assert(r.match("abcdef"))
        @assert(!r.match("AB"))
    }

    {
        var r: Parser.RegExp
        assume r.compile("\\p{Lu}*")
        @assert(r.match("ABCDE"))
        @assert(!r.match("ab"))
    }

    {
        var r: Parser.RegExp
        assume r.compile("\\pS*")
        @assert(r.match("£"))
        @assert(!r.match("a"))
    }

    {
        var r: Parser.RegExp
        assume r.compile("\\p{Sm}*")
        @assert(r.match("++"))
    }
}