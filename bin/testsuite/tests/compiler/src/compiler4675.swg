//#global skip

//#[Swag.PrintBc]
#test
{
    {
        const vv: any = 6
        #assert dref cast(*s32) @dataof(vv) == 6
    }
}