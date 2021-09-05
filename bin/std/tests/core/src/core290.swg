using Core

#test
{
    {
        var r: Parser.RegExp
        assume r.compile("[abc]")
        @assert(r.match("a"))
        @assert(r.match("b"))
        @assert(r.match("c"))
    }
}