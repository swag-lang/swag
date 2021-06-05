#global testerror

#test
{
    #[Swag.safety("overflow", true)]
    var x: u64 = 1 + Swag.U64.Max
}