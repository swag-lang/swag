//#[Swag.PrintBc]
#test
{
    var r: Core.Parser.RegExp
    assume r.compile(#"a(b|s)"#)
    @assert(r.match("ab"))
    @assert(r.match("as"))
}