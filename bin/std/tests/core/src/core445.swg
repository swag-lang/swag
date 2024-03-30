using Core
#test
{
    {
        var r = assume Parser.RegExp.create(#"([a-z])\1"#)
        @assert(r.match("aa"))
        @assert(!r.match("ab"))
    }

    {
        var r = assume Parser.RegExp.create(#"\/"#)
        @assert(r.match("/"))
    }

    {
        var r = assume Parser.RegExp.create(#"<(\w+)><\/\1>"#)
        @assert(r.match("<aa></aa>"))
    }

    {
        var r = assume Parser.RegExp.create(#"X\s*\s*X"#)
        @assert(r.match("XX"))
        @assert(r.match("X X"))
        @assert(r.match("X         X"))
    }
}
