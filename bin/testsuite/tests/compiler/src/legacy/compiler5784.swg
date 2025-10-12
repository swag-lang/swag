//#global skip
//#global testerror "Saf0030"

#[Swag.NoInline]
func titi()->#null *s32
{
    return null
}

#[Swag.Sanity(true), Swag.Safety(.All, .Null, true), Swag.Optimize("", false)]
//#[Swag.PrintGenBc]
func toto()
{
    let p1 = titi()
    @assert(p1 != null)
    dref p1 = 0
}
