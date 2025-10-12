#global testerror "Saf0027"

#[Swag.Safety(.All, .All, true), Swag.Optimize(.All, false)]
func toto(a: s16)
{
    let b = -1's16
    let c = a / b
    @print(c)
}

#test
{
    toto(Swag.S16.Min)
}
