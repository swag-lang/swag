#global testerror "Saf0034"

#[Swag.Optimize(.All, false), Swag.Sanity(true)]
func toto(v: bool)
{
    var a, b: s32 = undefined
    if v do
        a = 0
    else do
        b = 0
    @assert(a == 0)
}
