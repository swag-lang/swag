#global testerror "Saf0014"
#[Swag.Safety("overflow", true)]
#[Swag.Optim("", false)]
#test
{
    var x: s32 = Swag.S32.Min
    x = x - 2
}
