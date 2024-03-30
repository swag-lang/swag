using Core

#test
{
    var r: Parser.RegExp

    {
        assume r.compile(#"^toto$"#)
        @assert(r.match("toto"))
        @assert(r.grep("toto") == "toto")
        @assert(r.grep("atoto") == null)
        @assert(r.grep("totoa") == null)
    }

    {
        assume r.compile(#"toto$"#)
        @assert(r.grep("aaaatoto") == "toto")
        @assert(r.grep(" toto ") == null)
    }

    {
        assume r.compile(#"toto"#)
        let res = r.grepAll("toto toto toto")
        @assert(res.count == 3)
    }

    {
        assume r.compile(#"^toto|toto$"#)
        let res = r.grepAll("toto toto toto")
        @assert(res.count == 2)
    }
}
