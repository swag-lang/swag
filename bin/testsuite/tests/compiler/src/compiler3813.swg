//#global testerror
struct A
{
    x: s32
}

func toto()->A
{
    return {}
}

func aa()
{
    with var a = toto()
    {
        .x = 1
    }

    @assert(a.x == 1)
}
