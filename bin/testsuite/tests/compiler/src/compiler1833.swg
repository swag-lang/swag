func toto()->s32
{
    return 0
}

#test
{
    @assert(@typeof(toto) != func()->s32)
    @assert(@kindof(toto) == func()->s32)
}