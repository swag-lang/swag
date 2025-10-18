#global testerror "Err0068"
#test
{
    const a: string = null
    #[Swag.Safety(.All, .All, true)]
    if a[0] == 0
    {
    }
}
