#global testerror "Err0578"

//#[Swag.PrintBc]
#test
{
    const array: [..] s32 = [1, 2, 3, 4]
    #assert(array[0] == 1)
    #assert(array[3] == 4)
}