#global testerror "Err0028"

#[Swag.Overload]
func toto(a: bool, x: s8) {}
#[Swag.Overload]
func toto(a: bool, x: f32) {}

#test
{
    toto(true, 0)
}