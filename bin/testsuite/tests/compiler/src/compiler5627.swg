#global testerror "Saf0027"

#[Swag.Optim("", false)]
#[Swag.Safety("", true)]
func toto(b: u32)
{
    let x: u8 = cast(u8) b
}

#test
{
    toto(500)
}