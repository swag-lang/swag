#global testerror "San0006"

//#[Swag.PrintBc]
#[Swag.Safety("sanity", true)]
#test
{
    var x : string
    #[Swag.Safety("", true)]
    let ptr = &x[1]
    @assert(dref ptr == `A`'u8)
}