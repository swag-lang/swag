#if false
{
    enum AA
    {
        T = 1
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