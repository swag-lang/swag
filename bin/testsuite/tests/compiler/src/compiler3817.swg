#global testerror "Err1180"
struct A
{
    x, y: s32
}

func toto()->A
{
    return {}
}

func aa()
{
    x, .y = 1
}