#global testerror "Saf0026"
#[Swag.Safety("", "", true), Swag.Optimize("", false)]
#test
{
    let x = 512'f32
    let y = cast(u8) x
}
