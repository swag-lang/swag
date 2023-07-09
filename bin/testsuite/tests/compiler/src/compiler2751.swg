#global testerror

#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    var x = Swag.F64.Max
    var y = cast(s64) x
}
