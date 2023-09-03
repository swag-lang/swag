#global testerror "Saf0008"

//#[Swag.PrintBc]
#test
{
    var x : string
    #[Swag.Safety("", true)]
    let ptr = &x[1]
    @assert(dref ptr == `A`'u8)
}