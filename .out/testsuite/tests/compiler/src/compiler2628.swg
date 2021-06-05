#if false
{
    struct AA
    {
        T = 1
    }
}
#else
{
    struct AA
    {
        R = 2
    }
}

#assert !#defined(AA.T)
#assert #defined(AA.R)