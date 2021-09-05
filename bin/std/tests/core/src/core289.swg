using Core

#test
{
    {
        var r: Parser.RegExp
        assume r.compile("[a-d]")
        @assert(r.match("a"))
        @assert(r.match("b"))
        @assert(r.match("c"))
        @assert(r.match("d"))
        @assert(!r.match("e"))
    }

    {
        var r: Parser.RegExp
        assume r.compile("[[:digit:]]")
        @assert(r.match("0"))
        @assert(r.match("1"))
        @assert(r.match("2"))
        @assert(r.match("3"))
        @assert(r.match("4"))
        @assert(r.match("5"))
        @assert(r.match("6"))
        @assert(r.match("7"))
        @assert(r.match("8"))
        @assert(r.match("9"))
    }

    {
        var r: Parser.RegExp
        assume r.compile("[[:hexa:]]")
        @assert(r.match("a"))
        @assert(r.match("A"))
        @assert(r.match("F"))
        @assert(r.match("0"))
        @assert(r.match("9"))
        @assert(!r.match("g"))
    }
}