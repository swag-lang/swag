#global testerror "Err0487"
#[Swag.Overload]
func toto(x: bool, y: f32) {}
#[Swag.Overload]
func toto(x: bool, y: bool) {}

#test
{
    toto(true)
}