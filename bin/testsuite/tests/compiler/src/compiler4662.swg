#global testerror "Err0605"
#test
{
    const a: string = null
    #[Swag.Safety("", true)]
    if a[0] == 0
    {
    }
}