using Core

#test
{
    var r: Parser.RegExp

    {
        assume r.compile(#"\btoto"#)
        @assert(r.match("toto"))
        @assert(r.grep(" toto").value == "toto")
        @assert(r.grep("atoto").value == null)
    }

    {
        assume r.compile(#"toto\b"#)
        @assert(r.grep("totoa").value == null)
        @assert(r.grep(" toto ").value == "toto")
    }

    {
        assume r.compile(#"\btoto\b"#)
        let res = r.grepAll("toto atoto totoi toto toto")
        @assert(res.count == 3)
        @assert(res[0].index == 0)
        @assert(res[1].index == 17)
        @assert(res[2].index == 22)
    }
}