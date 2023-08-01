struct(T, loc: Loc) TOTO
{
    #ast
    {
        //@compilererror(#location, "this is a test man !")
        @assert(cast(s64) loc.lineStart == 12)
        return ""
    }
}

#test
{
    var x: TOTO'(s32, #location){}
}

typealias Loc = Swag.SourceCodeLocation
