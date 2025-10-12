#global testerror "Saf0027"
#global testerror "Saf0027"
#[Swag.Safety(.All, .Overflow, true), Swag.Optimize(.All, false)]
#test
{
    var x: u64 = Swag.U64.Max
    x = x * 2
}
