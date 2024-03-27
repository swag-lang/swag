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
    var a: A
    with a = toto()
    {
        .x = 1
    }

    @assert(a.x == 1)
}