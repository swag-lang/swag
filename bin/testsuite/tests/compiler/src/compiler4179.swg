//#global skip
#global testerror "San0001"

#[Swag.Sanity(true)]
func toto(str: string, x: cvarargs)
{
    var valist: Swag.CVaList
    var ptr = &valist
    ptr = null
    @cvastart(ptr)
}
