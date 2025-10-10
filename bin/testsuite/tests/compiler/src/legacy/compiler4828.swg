#global testerror "Saf0027"
#global testerror "Saf0027"
#[Swag.Safety("overflow", true)]
#[Swag.Optimize("", false)]
#test
{
    var x: s64 = Swag.S64.Max
    x = x * 2
}
