struct AA
{
    x, y: s32
}

var g: [2] AA = @[@{1, 2}, @{3, 4}]

#test
{
    @assert(g[0].x == 1)
    @assert(g[0].y == 2)
    @assert(g[1].x == 3)
    @assert(g[1].y == 4)
}