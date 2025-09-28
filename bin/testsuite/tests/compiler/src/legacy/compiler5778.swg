alias HookEvent = func||()->bool

#[Swag.Optimize("", false)]
//#[Swag.PrintGenBc]
func toto(x: HookEvent = null)
{
    var y: HookEvent = x
}