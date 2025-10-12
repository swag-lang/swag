#global testerror "Saf0027"
#global testerror "Saf0027"
#[Swag.Safety(.All, .Overflow, true)]
#test
{
    #[Swag.Safety(.All, .Overflow, true)]
    var x: u64 = Swag.U64.Max
    x *= 2
}
