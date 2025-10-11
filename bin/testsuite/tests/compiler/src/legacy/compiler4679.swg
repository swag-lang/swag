//#[Swag.PrintBc]
#test
{
    {
        const vv: [2] s32 = [1, 2]
        @assert(dref @dataof(vv) == 1)
        #assert(dref @dataof(vv) == 1)
    }
}
