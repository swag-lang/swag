
#global testerror

//#[Swag.PrintBc]
#test
{
    var x : string
    #[Swag.Safety("", true)]
    ptr := &x[1]
    @assert(:ptr == "A"'u8)
}