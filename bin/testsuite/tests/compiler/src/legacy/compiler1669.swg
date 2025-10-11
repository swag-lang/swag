func function(x: s32, y: ...)
{
    @assert(x == 50)
}

#run
{
    let ptr = &function
    ptr(50)
}
