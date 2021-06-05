var fn = @(x, y: s32) => x + y
#test
{
    @assert(fn(2,3)==5)
    fn = @(x, y: s32) => x * y
    @assert(fn(2,3)==6)
}