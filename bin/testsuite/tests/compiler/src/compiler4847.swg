//#global testerror "Err0177"
#test
{
    const PowerOfTwo: [4] s32 = #run
        {
            a := 1
            b := a + 1
            return [a, b, 3, 4]
        }
    #assert PowerOfTwo[0] == 1
    #assert PowerOfTwo[1] == 2
    #assert PowerOfTwo[3] == 4
}