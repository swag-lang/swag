#global testerror "Err0095"
#[Swag.Overload]
func toto(x, y: s32, z: f32) {}
#[Swag.Overload]
func toto(x, y, z: s32) {}

#test
{
    toto(0, 1, true)
}
