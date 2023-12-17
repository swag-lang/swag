using Core

#test
{
    var r: Parser.RegExp
    assume r.compile(#"^toto$"#)
    @assert(r.match("toto"))
    @assert(r.grep("toto").value == "toto")
    @assert(r.grep("atoto").value == null)
    @assert(r.grep("totoa").value == null)

    assume r.compile(#"toto$"#)
    @assert(r.grep("aaaatoto").value == "toto")
}