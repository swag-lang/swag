struct(T, loc: Loc) TOTO
{
    #ast
    {
        //@compilererror(#curlocation, "this is a test man !")
        @assert(cast(s64) loc.lineStart == 12)
        return ""
    }
}

#test
{
    var x: TOTO'(s32, #curlocation){}
}

alias Loc = Swag.SourceCodeLocation
