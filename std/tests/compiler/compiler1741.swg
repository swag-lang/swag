//#[swag.printbc]
#test
{
    {
        const array: [2] s32 = @[1, 2]
        ptr := @dataof(array)
        @assert(ptr[0] == 1)
        @assert(ptr[1] == 2)
        @assert(@countof(array) == 2)

        /*result := 0
        visit it: array
        {
            result += it
        }

        @assert(result == 1+2)*/
    }
}