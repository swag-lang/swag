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
        assume r.compile("\\d")
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
        assume r.compile("[[:xdigit:]]")
        @assert(r.match("a"))
        @assert(r.match("A"))
        @assert(r.match("F"))
        @assert(r.match("0"))
        @assert(r.match("9"))
        @assert(!r.match("g"))
    }

    {
        var r: Parser.RegExp
        assume r.compile("[[:alpha:]]")
        @assert(r.match("a"))
        @assert(r.match("z"))
        @assert(r.match("A"))
        @assert(r.match("Z"))
        @assert(!r.match("0"))
    }

    {
        var r: Parser.RegExp
        assume r.compile("[[:space:]]")
        @assert(r.match(" "))
        @assert(!r.match("z"))
        @assert(r.match("\t"))
        @assert(r.match("\n"))
    }
}