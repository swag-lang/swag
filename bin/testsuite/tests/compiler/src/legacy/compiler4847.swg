//#global testerror "Err0530"
#test
{
    const PowerOfTwo: [4] s32 = #run
        {
            let a = 1
            let b = a + 1
            return [a, b, 3, 4]
        }
    #assert(PowerOfTwo[0] == 1)
    #assert(PowerOfTwo[1] == 2)
    #assert(PowerOfTwo[3] == 4)
}