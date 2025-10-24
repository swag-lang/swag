#global testerror "Err0570"

#[Swag.Safety(.All, .All, true)]
func toto(a: s32)
{
    let b = -1
    let c = a / b
    @print(c)
}

#test
{
    let x = Swag.S64.Min / -1
    //toto(Swag.S32.Min)
}
