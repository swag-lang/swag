#global testerror

//#[Swag.PrintBc]
#test
{
    var x : string
    #[Swag.Safety("", true)]
    var ptr = &x[1]
    @assert(dref ptr == "A"'u8)
}