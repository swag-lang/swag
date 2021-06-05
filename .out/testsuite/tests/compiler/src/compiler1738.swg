
#global testerror

#[Swag.printbc]
#test
{
    {
        const array: [2] s32 = [1, 2]
        ptr := @dataof(array)
        ptr[0] = 0

        //@assert(ptr[0] == 1)

        /*result := 0
        visit it: array
        {
            result += it
        }

        @assert(result == 1+2)*/
    }
}