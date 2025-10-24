#global testerror "Err0068"
#test
{
    const a: [..] s32 = null
    #[Swag.Safety(.All, .All, true)]
    if a[0] == 0
    {
    }
}
