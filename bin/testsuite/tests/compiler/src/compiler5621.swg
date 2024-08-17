#global testerror "Saf0027"

#[Swag.Safety("", true)]
//#[Swag.PrintBc]
#[Swag.Optim("", false)]
func toto(a: s32)
{
    let b = -1
    var c = a
    c /= b
    @print(c)
}

#test
{
    toto(Swag.S32.Min)
}
