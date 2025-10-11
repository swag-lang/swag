#global testerror "Saf0020"
#[Swag.Safety("", true), Swag.Optimize("", false)]
#test
{
    let x = -1's8
    let y = cast(u16) x
}
