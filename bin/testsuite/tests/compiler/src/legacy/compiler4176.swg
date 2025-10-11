#global testerror "Saf0034"

#[Swag.Optimize("", false), Swag.Sanity(true)]
func toto(v: bool)
{
    var a, b: s32 = undefined
    if v:
        a = 0
    else:
        b = 0
    @assert(a == 0)
}
