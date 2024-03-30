using Core
#test
{
    {
        var r = assume Parser.RegExp.create(#".*XY"#)
        @assert(r.match("tXY"))
    }

    {
        var r = assume Parser.RegExp.create(#"<(\w+)>.*<<\1>>"#)
        @assert(r.match("<aa><<aa>>"))
    }

    {
        var r = assume Parser.RegExp.create(#"(\b\w+\b)\s+\1"#)
        @assert(r.match("hello hello"))
        @assert(r.match("123 123"))
        @assert(!r.match("hello world"))
    }

    {
        var r = assume Parser.RegExp.create(#"(\w)\1+"#)
        @assert(r.match("aa"))
        @assert(!r.match("ab"))
    }
}
