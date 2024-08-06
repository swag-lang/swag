#global testerror "Err0565"
#[Swag.Overload]
func toto(x: bool, y: f32) {}
#[Swag.Overload]
func toto(x: bool, y: bool) {}

#test
{
    toto(true)
}