#global testerror

//#[Swag.PrintBc]
#[Swag.Optim("", false)]
#[Swag.Safety("sanity", true)]
#[Swag.Safety("overflow", true)]
#test
{
    x := 1'u8
    y := 1'u8
    x >>= y
    x >>= y
}