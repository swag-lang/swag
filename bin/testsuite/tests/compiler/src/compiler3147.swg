func toto(x: cstring)
{
    len := @strlen(x)
    @assert(len == 4)
    str := @mkstring(x, len)
    @assert(str == "titi")
}

#test
{
    toto("titi")
}