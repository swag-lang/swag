#global testerror "Saf0012"

#test
{
    #[Swag.Safety("overflow", true)]
    var x: u64 = 1 + Swag.U64.Max
}