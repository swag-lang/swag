#global testerror "Err0530"

enum AA: const [2] s32
{
    A = [1, 2]
    B = [10, 20]
}

#test
{
    var x = AA.A
    switch x
    {
    case [1, 2]: break
    }
}