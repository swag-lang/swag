#global testerror "Err0024"

#[Swag.Overload]
func toto(x: s8) {}
#[Swag.Overload]
func toto(x: f32) {}

#test
{
    toto(0)
}
