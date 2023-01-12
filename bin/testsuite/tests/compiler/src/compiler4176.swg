#global testerror

//#[Swag.PrintBc]
#[Swag.Optim("", false)]
#[Swag.Safety("sanity", true)]
func toto(v: bool)
{
    var a, b: s32 = ?
    if v
        a = 0
    else
        b = 0
    @assert(a == 0)
}