#global testerror "Saf0026"
#[Swag.Safety("", "", true), Swag.Optimize("", false)]
#test
{
    let x = -255'f64
    let y = cast(s8) x
}
