#global testerror "Err0218"
#test
{
    const a: string = null
    #[Swag.Safety("", "", true)]
    if a[0] == 0
    {
    }
}
