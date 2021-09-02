#global testerror

#test
{
    #[Swag.Safety("overflow", true)]
    var x: s64 = Swag.S64.Max + 1
}