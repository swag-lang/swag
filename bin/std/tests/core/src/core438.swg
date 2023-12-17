using Core

#test
{
    var r: Parser.RegExp
    assume r.compile(#"^toto$"#)
    @assert(r.match("toto"))
    @assert(r.grep("toto").value == "toto")
    @assert(r.grep("atoto").value == null)
    @assert(r.grep("totoa").value == null)
    //@print(assume r.match(#"^toto"#, "toto"))
}