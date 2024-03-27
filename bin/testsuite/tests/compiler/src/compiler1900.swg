struct(T, loc: s64 /*Swag.SourceCodeLocation*/) TOTO
{
    #ast
    {
        //@compilererror(#location, "this is a test man !")
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
