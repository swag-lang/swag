#global testerror "Err0095"
#[Swag.Overload]
func toto(x: bool, y: f32) {}
#[Swag.Overload]
func toto(x: bool, y: bool) {}

#test
{
    toto(true)
}
