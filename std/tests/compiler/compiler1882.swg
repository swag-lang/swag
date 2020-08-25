struct(T) AA
{
    #assert(@typeof(T) == s32)
    x: T
}

#test
{
    var x: AA's32
    @assert(x.x == 0)
}