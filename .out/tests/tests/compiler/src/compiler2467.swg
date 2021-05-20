#global testerror

#test
{
    #[Swag.safety("overflow", true)]
    var x: s64 = Swag.S64.Min - 1
}