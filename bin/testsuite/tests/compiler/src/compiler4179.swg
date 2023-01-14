#global testerror

#[Swag.Safety("sanity", true)]
func toto(str: string, x: cvarargs)
{
    var valist: Swag.CVaList
    ptr := &valist
    ptr = null
    @cvastart(ptr)
}
