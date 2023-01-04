#global testerror

#test
{
    #[Swag.Safety("over", true)]
    var x: u64 = 0'u64 - 1
}