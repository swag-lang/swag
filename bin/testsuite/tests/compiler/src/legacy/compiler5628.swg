#global testerror "Saf0028"

#[Swag.Optimize("", false), Swag.Safety(.All, "", true)]
func toto(b: u32)
{
    let x: s32 = cast(s32) b
}

#test
{
    toto(Swag.U32.Max)
}
