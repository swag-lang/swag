func toto()->typeinfo
{
    return s32
}

#test
{
    @assert(toto() == s32)
}
