struct A { x, y: s32 }

#test
{
    with var a: A
    {
        .x = 1
        .y = 2
    }

    @assert(a.x == 1)
    @assert(a.y == 2)
}
