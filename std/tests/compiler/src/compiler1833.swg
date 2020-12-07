func toto()->s32
{
    return 0
}

#test
{
    @assert(@kindof(toto) == s32)
}