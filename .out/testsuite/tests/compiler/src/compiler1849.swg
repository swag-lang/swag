
#global testerror

//#[Swag.PrintBc]
#test
{
    var x : string
    #[Swag.safety("", true)]
    ptr := &x[1]
    @assert(:ptr == "A"'u8)
}