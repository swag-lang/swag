#global testerror "Err0093"

//#[Swag.PrintBc]
#test
{
    {
        const array: [2] s32 = [1, 2]
        var ptr = @dataof(array)
        ptr[0] = 0

        //@assert(ptr[0] == 1)

        /*var result = 0
        foreach it in array
        {
            result += it
        }

        @assert(result == 1+2)*/
    }
}