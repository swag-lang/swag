#global testerror "Err0132"

enum AA: const [2] s32
{
    A = [1, 2]
    B = [10, 20]
}

#test
{
    foreach &x in AA
    {
    }
}