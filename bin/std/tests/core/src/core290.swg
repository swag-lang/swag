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

    {
        var r: Parser.RegExp
        assume r.compile("[[:lower:]]*")
        @assert(r.match("abcde"))
        @assert(!r.match("A"))
    }

    {
        var r: Parser.RegExp
        assume r.compile("[[:upper:]]*")
        @assert(r.match("ABCDE"))
        @assert(!r.match("a"))
    }

    {
        var r: Parser.RegExp
        assume r.compile("\\w+")
        @assert(r.match("A4_bcde"))
    }

    {
        var r: Parser.RegExp
        assume r.compile("[[:space:]]*")
        @assert(r.match("   \n"))
    }

    {
        var r: Parser.RegExp
        assume r.compile("[[:ascii:]]*")
        @assert(r.match("abcDEGe4mffrpt"))
        @assert(!r.match("大"))
    }
}
