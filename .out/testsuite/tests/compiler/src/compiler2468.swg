#global testerror

#test
{
    #[Swag.safety("overflow", true)]
    var x: u64 = 0'u64 - 1
}