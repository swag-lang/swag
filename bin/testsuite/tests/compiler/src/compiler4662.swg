#global testerror "Err0545"
#test
{
    const a: string = null
    #[Swag.Safety("", true)]
    if a[0] == 0
    {
    }
}