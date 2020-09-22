struct(T, loc: swag.CompilerSourceLocation) TOTO
{
    #ast
    {
        //@error(#location, "this is a test man !")
        @assert(cast(s64) loc.lineStart == 12)
        return ""
    }
}

#test
{
    var x: TOTO'(s32, #location){}
}