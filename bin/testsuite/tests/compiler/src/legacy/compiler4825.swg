#global testerror "Saf0027"
#global testerror "Saf0027"
#[Swag.Safety(.All, .Overflow, true), Swag.Optimize("", false)]
#test
{
    var x: s64 = Swag.S64.Min
    x = x - 2
}
