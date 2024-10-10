#global testerror "Err0218"
//#global skip

//#[Swag.PrintBc]
#test
{
    {
        const vv: any = null
        #assert dref cast(*s32) @dataof(vv) == 6
    }
}