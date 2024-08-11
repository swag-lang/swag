#global testerror "Err0009"

#[Swag.Overload]
func toto(a: bool, x: s8) {}
#[Swag.Overload]
func toto(a: bool, x: f32) {}

#test
{
    toto(true, 0)
}