#global testerror "San0006"

#[Swag.Safety("sanity", true)]
func toto(str: string, x: cvarargs)
{
    var valist: Swag.CVaList
    var ptr     = &valist
    ptr = null
    @cvastart(ptr)
}
