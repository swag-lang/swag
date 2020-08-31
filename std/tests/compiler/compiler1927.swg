alias A = [2] s32
var x: A

#test
{
    x[0] = 1
    @assert(@countof(x) == 2)
}