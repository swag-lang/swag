#[swag.printbc]
#test
{
    {
        const array: [2] s32 = [1, 2]

        ptr := cast(*s32) @dataof(array)
        //@assert(ptr[0] == 1)

        /*result := 0
        visit it: array
        {
            result += it
        }

        @assert(result == 1+2)*/
    }
}