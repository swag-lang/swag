#global testerror "Err0004"

#[Swag.Overload]
func toto(x: s8) {}
#[Swag.Overload]
func toto(x: f32) {}

#test
{
    toto(0)
}
