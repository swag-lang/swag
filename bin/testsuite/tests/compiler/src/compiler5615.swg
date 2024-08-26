#global testerror "Err0055"

#[Swag.Safety("", true)]
func toto(a: s32)
{
    let b = -1
    let c = a / b
    @print(c)
}

#test
{
    let x = Swag.S16.Min / -1
    //toto(Swag.S32.Min)
}
