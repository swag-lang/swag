#global testerror "Err0702"

struct AA
{
    x:   s32
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
