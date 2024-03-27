#global testerror "San0010"
#global testerror "Saf0026"
#[Swag.Safety("overflow", true)]
#[Swag.Optim("", false)]
#test
{
    var x: u32 = Swag.U32.Max
    x = x * 2
}
