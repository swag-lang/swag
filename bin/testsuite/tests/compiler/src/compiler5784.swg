//#global skip
//#global testerror "San0001"

#[Swag.NoInline]
func titi()->#null *s32
{
    return null
}

#[Swag.Sanity(true)]
#[Swag.Safety("null", true)]
#[Swag.Optimize("", false)]
//#[Swag.PrintGenBc]
func toto()
{
    let p1 = titi()
    @assert(p1 != null)
    dref p1 = 0
}