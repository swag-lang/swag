//#[Swag.PrintBc]
#test
{
    {
        const array: [2] s32 = [1, 2]
        let ptr      = @dataof(array)
        @assert(ptr[0] == 1)
        @assert(ptr[1] == 2)
        @assert(@countof(array) == 2)
    }
}
