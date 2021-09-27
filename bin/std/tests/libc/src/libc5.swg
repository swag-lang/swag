using Libc

#test
{
    var x: f32
    var y: s32
    sscanf("1.2,666", "%f,%d", &x, &y)
    @assert(x == 1.2)
    @assert(y == 666)
}