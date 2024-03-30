using Core

#test
{
    var r: Parser.RegExp

    {
        assume r.compile(#"\btoto"#)
        @assert(r.match("toto"))
        @assert(r.grep(" toto") == "toto")
        @assert(r.grep("atoto") == null)
    }

    {
        assume r.compile(#"toto\b"#)
        @assert(r.grep("totoa") == null)
        @assert(r.grep(" toto ") == "toto")
    }

    {
        assume r.compile(#"\btoto\b"#)
        let res = r.grepAll("toto atoto totoi toto toto")
        @assert(res.count == 3)
    }
}
