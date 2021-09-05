using Core

#test
{
    var r: Parser.RegExp
    assume r.compile("ab")
    @assert(r.match("ab"))
}