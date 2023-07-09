func function(x: s32, y: ...)
{
    @assert(x == 50)
}

#run
{
    var ptr = &function
    ptr(50)
}