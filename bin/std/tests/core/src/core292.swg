using Core

#test
{
    {
        var r: Parser.RegExp
        assume r.compile("\\pL*")
        @assert(r.match("a"))
        @assert(r.match("目目目"))
    }
}