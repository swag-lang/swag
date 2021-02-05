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
    (x, y) := titi()
}