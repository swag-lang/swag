//#[swag.printbc]
#test
{
    {
        const array: [2] s32 = @[1, 2]

        result := 0
        visit it: array
        {
            result += it
        }

        @assert(result == 1+2)
    }
}