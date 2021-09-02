#if false
{
    enum AA
    {
        T = 1
        R = 2
    }
}
#else
{
    enum AA
    {
        T = 2
    }
}

#assert AA.T == 2
#assert !#defined(AA.R)