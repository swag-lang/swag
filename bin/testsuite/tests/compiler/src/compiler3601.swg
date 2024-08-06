#global testerror "Err0573"
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