#global testerror "San0009"
#[Swag.Safety("overflow", true)]
#test
{
    #[Swag.Safety("overflow", true)]
    var x: u64 = Swag.U64.Max
    x *= 2
}
