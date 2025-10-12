#global testerror "Saf0028"

#[Swag.Optimize("", false), Swag.Safety("", "", true)]
func toto(b: u32)
{
    let x: u8 = cast(u8) b
}

#test
{
    toto(500)
}
