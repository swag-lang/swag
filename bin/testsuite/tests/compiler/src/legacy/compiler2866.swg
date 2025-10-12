#global testerror "Saf0024"

//#[Swag.PrintBc]
#[Swag.Safety(.All, .All, true), Swag.Optimize(.All, false)]
#test
{
    let x  = 0xFFFFFFFF'u32
    let y: s32 = x
}
