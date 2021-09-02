#global testerror

enum AA: const [2] s32
{
    A = @[1, 2]
    B = @[10, 20]
}

#test
{
    x := AA.A
    switch x
    {
    case @[1, 2]: break
    }
}