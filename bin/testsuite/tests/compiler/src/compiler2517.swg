#global testerror "Err0724"

struct AA
{
    x: s32
}

func toto()->AA
{
    var result: retval
    return result
}

#test
{
    var (x, y) = toto()
}