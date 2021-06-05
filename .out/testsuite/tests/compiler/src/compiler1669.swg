func function(x: s32, y: ...)
{
    @assert(x == 50)
}

#run
{
    ptr := &function
    ptr(50)
}