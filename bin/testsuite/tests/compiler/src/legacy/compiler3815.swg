#global testerror "Err0060"
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
    with var a, b = toto()
    {
        .x = 1
    }
}