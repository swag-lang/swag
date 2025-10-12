#global testerror "Err0218"
#test
{
    const a: [..] s32 = null
    #[Swag.Safety(.All, "", true)]
    if a[0] == 0
    {
    }
}
