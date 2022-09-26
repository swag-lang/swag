struct A
{
    x, y: s32
}

func toto()->A
{
    return @{}
}

func aa()
{
    with a := toto()
    {
        .x, .y = 1
    }

    @assert(a.x == 1 and a.y == 1)
}