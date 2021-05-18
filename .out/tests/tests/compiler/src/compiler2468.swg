#global testerror

#test
{
    #[swag.safety("overflow", true)]
    var x: u64 = 0'u64 - 1
}