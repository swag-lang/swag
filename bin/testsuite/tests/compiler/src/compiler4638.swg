#global testerror "Err0599"
#test
{
    const a: [..] s32 = null;
    #[Swag.Safety("", true)]
    if a[0] == 0
    {
    }
}