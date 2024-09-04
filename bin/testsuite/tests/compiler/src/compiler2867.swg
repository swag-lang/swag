#global testerror "Err0518"

//#[Swag.PrintBc]
#[Swag.Safety("", true)]
#[Swag.Optimize("", false)]
#test
{
    var x: u8 = 0
    x += 10'f32
}