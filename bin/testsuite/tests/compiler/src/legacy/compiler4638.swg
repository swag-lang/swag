#global testerror "Err0070"
#test
{
    const a: [..] s32 = null
    #[Swag.Safety(.All, .All, true)]
    if a[0] == 0
    {
    }
}
