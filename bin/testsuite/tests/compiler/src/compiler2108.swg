struct AA
{
    x, y: s64
}

const v: AA{10, 20}
#assert v.x == 10
#assert v.y == 20

var vv: AA = v

#test
{
    @assert(vv.x == 10)
    @assert(vv.y == 20)
}