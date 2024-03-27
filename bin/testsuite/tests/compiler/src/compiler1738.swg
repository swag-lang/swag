#global testerror "Err0104"

//#[Swag.PrintBc]
#test
{
    {
        const array: [2] s32 = [1, 2]
        var ptr      = @dataof(array)
        ptr[0] = 0
    }
}
