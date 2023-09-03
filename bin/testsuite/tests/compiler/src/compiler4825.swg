#global testerror "Saf0014"
#[Swag.Safety("overflow", true)]
#[Swag.Optim("", false)]
#test
{
    var x: s64 = Swag.S64.Min
    x = x - 2
}
