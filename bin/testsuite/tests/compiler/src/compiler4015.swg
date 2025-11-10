struct A { x, y: s16 }

struct B { x, y: s8 }

#test
{
    var v: A = {0, 2}
    @assert(v.x == 0)
    @assert(v.y == 2)

    var v1: B = {0, 2}
    @assert(v1.x == 0)
    @assert(v1.y == 2)
}
