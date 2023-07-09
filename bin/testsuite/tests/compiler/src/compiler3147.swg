func toto(x: cstring)
{
    var len = @strlen(x)
    @assert(len == 4)
    var str = @mkstring(x, len)
    @assert(str == "titi")
}

#test
{
    toto("titi")
}