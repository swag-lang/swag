#global testerror "Err0597"
#test
{
    const a: string = null
    #[Swag.Safety("", true)]
    if a[0] == 0
    {
    }
}