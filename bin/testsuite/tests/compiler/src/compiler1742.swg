//#[Swag.PrintBc]
#test
{
    {
        const array: [2] s32 = [1, 2]

        var result = 0
        foreach it in array
        {
            result += it
        }

        @assert(result == 1 + 2)
    }
}
