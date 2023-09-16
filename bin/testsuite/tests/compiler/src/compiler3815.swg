#global testerror "Err1157"
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