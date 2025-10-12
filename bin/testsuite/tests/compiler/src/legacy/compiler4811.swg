#global testerror "Saf0026"
#[Swag.Safety("", "", true), Swag.Optimize("", false)]
#test
{
    let x = 512'f64
    let y = cast(u8) x
}
