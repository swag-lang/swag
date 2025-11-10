struct V { x, y: f32 }

func toto(ptr: #null *void, p0, p1, p2, p3: V)
{
    @assert(p3.x == 4)
    @assert(p3.y == 4)
}

#test
{
    toto(null, {1, 1}, {2, 2}, {3, 3}, {4, 4})
}
