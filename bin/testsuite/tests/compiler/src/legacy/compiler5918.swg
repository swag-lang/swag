//#global skip
//#global testerror "San0002"

//#[Swag.PrintGenBc]
#[Swag.Sanity(true)]
#[Swag.Optimize("", false)]
func titi()
{
    const A = 1.0 / Swag.F32.NaN
}
