#global testerror "Saf0027"

#test
{
    #[Swag.Safety("overflow", true)]
    var x: u64 = 1 + Swag.U64.Max
}