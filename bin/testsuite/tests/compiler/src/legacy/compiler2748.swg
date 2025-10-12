#global testerror "Saf0026"
#[Swag.Safety("", "", true), Swag.Optimize("", false)]
#test
{
    let x = 100000'f32
    let y = cast(u16) x
}
