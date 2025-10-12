#global testerror "Saf0027"
#global testerror "Saf0027"
#[Swag.Safety(.All, "overflow", true), Swag.Optimize("", false)]
#test
{
    var x: u64 = Swag.U64.Max
    x = x * 2
}
