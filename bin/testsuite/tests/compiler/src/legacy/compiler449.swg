//#[Swag.PrintBc]
#global testerror "Saf0018"
#[Swag.Sanity(true)]
func test()
{
    let a: [..] s32 = null
    #[Swag.Safety(.All, .All, true)]
    if a[0] == 0
    {
    }
}
