#global testerror "Saf0027"

#[Swag.Optim("", false)]
#[Swag.Safety("", true)]
func toto(b: u32)
{
    let x: s32 = cast(s32) b
}

#test
{
    toto(Swag.U32.Max)
}