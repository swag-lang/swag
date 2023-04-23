#global testerror

#test
{
    #[Swag.Safety("overflow", true)]
    var x: u64 = 0'u64 - 1
}