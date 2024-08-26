#global testerror "Err0144"
func printf(args: cvarargs)
{
    var valist: Swag.CVaList
    @cvastart(&valist)
    var x = @cvaarg(&valist, f32)
    @cvaend(&valist)
}
