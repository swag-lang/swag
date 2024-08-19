#global testerror "Err0121"
func printf(args: cvarargs)
{
    var valist: Swag.CVaList
    @cvastart(&valist)
    var x = @cvaarg(&valist, f32)
    @cvaend(&valist)
}
