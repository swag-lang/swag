#global testerror "San0009"
#global testerror "Saf0010"
#[Swag.Safety("overflow", true)]
#[Swag.Optim("", false)]
#test
{
    var x: u64 = Swag.U64.Max
    x = x * 2
}
