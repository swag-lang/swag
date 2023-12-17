using Core

#test
{
    var r: Parser.RegExp

    {
        assume r.compile(#"^toto$"#)
        @assert(r.match("toto"))
        @assert(r.grep("toto").value == "toto")
        @assert(r.grep("atoto").value == null)
        @assert(r.grep("totoa").value == null)
    }

    {
        assume r.compile(#"toto$"#)
        @assert(r.grep("aaaatoto").value == "toto")
        @assert(r.grep(" toto ").value == null)
    }

    {
        assume r.compile(#"toto"#)
        let res = r.grepAll("toto toto toto")
        @assert(res.count == 3)
        @assert(res[0].index == 0)
        @assert(res[1].index == 5)
        @assert(res[2].index == 10)
    }

    {
        assume r.compile(#"^toto|toto$"#)
        let res = r.grepAll("toto toto toto")
        @assert(res.count == 2)
        @assert(res[0].index == 0)
        @assert(res[1].index == 10)
    }
}