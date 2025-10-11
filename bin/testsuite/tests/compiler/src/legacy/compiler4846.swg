#global testerror "Err0530"
#test
{
    const PowerOfTwo: [4] f32 = #run
    {
        return [1, 2, 3, 4]
    }
    #assert(PowerOfTwo[0] == 1)
    #assert(PowerOfTwo[3] == 4)
}
