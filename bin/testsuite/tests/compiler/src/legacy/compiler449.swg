//#[Swag.PrintBc]
#global testerror "Saf0018"
#[Swag.Sanity(true)]
#test
{
    let a: [..] s32 = null
    #[Swag.Safety("", true)]
    if a[0] == 0
    {
    }
}
