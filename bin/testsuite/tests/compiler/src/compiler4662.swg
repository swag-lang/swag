#global testerror "Err0201"
#test
{
    const a: string = null
    #[Swag.Safety("", true)]
    if a[0] == 0
    {
    }
}