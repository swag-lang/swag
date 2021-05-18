#global testerror

#test
{
    #[swag.safety("overflow", true)]
    var x: u64 = 1 + swag.U64.Max
}