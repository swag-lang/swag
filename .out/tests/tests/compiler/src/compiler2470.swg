#global testerror

#test
{
    #[swag.safety("overflow", true)]
    var x: s64 = swag.S64.Min * 2
}