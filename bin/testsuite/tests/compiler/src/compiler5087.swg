#global testerror "Err0418"

enum AA: const [2] s32
{
    A = [1, 2]
    B = [10, 20]
}

#test
{
    visit &x: AA
    {
    }
}