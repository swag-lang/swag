#global testerror "San0010"
#global testerror "Saf0026"
#[Swag.Safety("overflow", true)]
#test
{
    #[Swag.Safety("overflow", true)]
    var x: u64 = Swag.U64.Max
    x *= 2
}
