#global testerror "Saf0004"

//#[Swag.PrintBc]
#[Swag.Sanity(false)]
#test
{
    var x: string
    #[Swag.Safety(.All, .All, true)]
    let ptr = &x[1]
    @assert(dref ptr == 'A''u8)
}
