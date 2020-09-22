struct(T, loc: s64 /*swag.CompilerSourceLocation*/) TOTO
{
    #ast
    {
        //@error(#location, "this is a test man !")
        //@print(loc.fileName)
        @assert(loc == 666)
        return ""
    }
}

#test
{
    //var x: TOTO'(s32, #location)()
    var x: TOTO'(s32, 666){}
}