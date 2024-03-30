using Core

#test
{
    {
        var r: Parser.RegExp
        assume r.compile("[-+]?([0-9]*[.])?[0-9]+([eE][-+]?\\d+)?")
        @assert(r.match("0.3"))
        @assert(r.match("-0.3e+2"))
    }
}
