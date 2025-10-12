#global testerror "Saf0021"

#[Swag.Safety(.All, .All, true), Swag.Optimize(.All, false)]
func toto(a: s32)
{
    let b: u8 = cast(u8) a
    @print(b)
}

#test
{
    toto(-50)
}
