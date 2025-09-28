#test
{
    enum A
    {
        AA
        BB
    }

    alias my = A
    const Z = A.AA
    #assert(Z == 0)
    #assert(A.AA == 0)
}