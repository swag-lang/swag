#global testerror
func printf(args: cvarargs)
{
    var valist: Swag.CVaList
    @cvastart(&valist)
    x := @cvaarg(&valist, f32)
    @cvaend(&valist)
}
