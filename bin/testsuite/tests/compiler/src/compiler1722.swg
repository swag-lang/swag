#test
{
    enum A
    {
        AA
        BB
    }

    typealias my = A
    const Z = A.AA
    #assert (Z == 0)
    #assert (A.AA == 0)
}
