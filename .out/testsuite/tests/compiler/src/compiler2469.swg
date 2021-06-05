#global testerror

#test
{
    #[Swag.safety("overflow", true)]
    var x: u64 = Swag.U64.Max * 2
}