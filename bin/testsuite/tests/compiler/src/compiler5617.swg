#global testerror "Saf0027"

#[Swag.Safety("", true)]
//#[Swag.PrintBc]
#[Swag.Optimize("", false)]
func toto(a: s32)
{
    let b = -1
    let c = a / b
    @print(c)
}

#test
{
    toto(Swag.S32.Min)
}
