#global testerror "Err0506"
#[Swag.Overload]
{
func toto(x: s32) {}
func toto(x: s64) {}
func toto(x: f32) {}
}
#test
{
    toto(true)
}