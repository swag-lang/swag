#global testerror "San0010"
#global testerror "Saf0026"
#[Swag.Safety("overflow", true)]
#[Swag.Optim("", false)]
#test
{
    var x: u64 = Swag.U64.Max
    x = x * 2
}
