#global testerror "Saf0027"

#[Swag.Safety(.All, .All, true), Swag.Optimize(.All, false)]
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
