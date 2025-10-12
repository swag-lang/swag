#global testerror "Saf0027"
#global testerror "Saf0027"
#[Swag.Safety(.All, "overflow", true)]
#test
{
    #[Swag.Safety(.All, "overflow", true)]
    var x: u64 = Swag.U64.Max
    x *= 2
}
