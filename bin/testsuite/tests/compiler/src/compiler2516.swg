#global testerror "Err0251"

struct AA
{
    x: s32
}

func toto()->AA
{
    var result: retval
    return result
}

func titi() => 0

#test
{
    var x, y: s32
    (x, y) = titi()
}