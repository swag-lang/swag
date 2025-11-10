//#[Swag.PrintBc]
#test
{
    {
        const vv: any = null
        @assert(@dataof(vv) == null)
        #assert(@dataof(vv) == null)
    }

    {
        const vv: any = 6
        @assert(dref cast(const *s32) @dataof(vv) == 6)
    }
}
