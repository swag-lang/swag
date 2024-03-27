//#global testerror

#[Swag.Safety("overflow", true), Swag.Optim("", false)]
#test
{
    const x = -1'f32
    let y   = cast,over(u64) x
}
