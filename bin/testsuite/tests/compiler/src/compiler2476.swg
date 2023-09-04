#global testerror "Saf0010"
#[Swag.Safety("overflow", true)]
#[Swag.Optim("", false)]
#test
{
    var x: s32 = Swag.S32.Max
    x = x * 2
}
