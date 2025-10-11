//#[Swag.PrintBc]
#test
{
    {
        const array: [2] s32 = [1, 2]
        let ptr      = @dataof(array)
        @assert(ptr[0] == 1)
        @assert(ptr[1] == 2)
        @assert(@countof(array) == 2)

        /*var result = 0
        foreach it in array
        {
            result += it
        }

        @assert(result == 1+2)*/
    }
}
