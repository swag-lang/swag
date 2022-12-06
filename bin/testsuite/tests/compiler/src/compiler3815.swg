#global testerror
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
    with a, b := toto()
    {
        .x = 1
    }
}