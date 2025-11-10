//#[Swag.PrintBc]
#test
{
    {
        const vv: const [..] s32 = null
        @assert(@dataof(vv) == null)
        #assert(@dataof(vv) == null)
    }

    {
        const vv: const [..] s32 = [1, 2]
        @assert(dref @dataof(vv) == 1)
        #assert(dref @dataof(vv) == 1)
    }
}
