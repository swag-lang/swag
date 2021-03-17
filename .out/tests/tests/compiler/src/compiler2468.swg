#global testerror

#test
{
    #[swag.safety("of", true)]
    var x: u64 = 0'u64 - 1
}