func toto(x: cstring)
{
    let len = @strlen(x)
    @assert(len == 4)
    let str = @mkstring(x, len)
    @assert(str == "titi")
}

#test
{
    toto("titi")
}
