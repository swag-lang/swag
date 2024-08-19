#global testerror "Err0739"

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