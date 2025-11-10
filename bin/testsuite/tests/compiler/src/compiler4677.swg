//#[Swag.PrintBc]
#test
{
    {
        const vv: string = null
        @assert(@dataof(vv) == null)
        #assert(@dataof(vv) == null)
    }

    {
        const vv: string = "null"
        @assert(dref cast(const *u8) @dataof(vv) == 'n''u8)
        #assert(dref cast(const *u8) @dataof(vv) == 'n''u8)
    }
}
