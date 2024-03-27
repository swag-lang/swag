func function(x: s32, y: s32 = 100)
{
    @assert(x == 50)
    @assert(y == 100)
}

#run
{
    let ptr = &function
    ptr(50)
}
