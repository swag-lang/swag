#global testerror "Err0530"

//#[Swag.PrintBc]
#[Swag.Safety("", true)]
#[Swag.Optimize("", false)]
#test
{
    var x: u8 = 0
    x += 10'f32
}