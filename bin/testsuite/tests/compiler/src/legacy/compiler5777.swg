//alias HookEvent = func||()->bool

#[Swag.Optimize("", false)]
//#[Swag.PrintGenBc]
func toto(x: func()->bool = null)
{
    var y: func||()->bool = x
}
