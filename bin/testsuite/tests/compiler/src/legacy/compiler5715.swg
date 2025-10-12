#global testerror "Saf0018"

//#[Swag.PrintBc]
#[Swag.Sanity(true)]
func test()
{
    var x: string
    #[Swag.Safety(.All, "", true)]
    let ptr = &x[1]
    @assert(dref ptr == 'A''u8)
}
