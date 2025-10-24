#global testerror "Err0023"

//#[Swag.PrintBc]
#[Swag.Safety(.All, .All, true), Swag.Optimize(.All, false)]
#test
{
    var x: u8 = 0
    x += 10'f32
}
