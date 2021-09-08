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
}