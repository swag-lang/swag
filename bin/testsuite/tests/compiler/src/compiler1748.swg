#global testerror "Err0025"

//#[Swag.PrintBc]
#test
{
    const array: [..] s32 = [1, 2, 3, 4]
    #assert(array[0] == 1)
    #assert(array[3] == 4)
}
