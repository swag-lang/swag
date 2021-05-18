#global testerror

#test
{
    #[swag.safety("overflow", true)]
    var x: u64 = swag.U64.Max * 2
}