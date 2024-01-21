#global testerror "Err0017"

#[Swag.Overload]
func toto(x: s32) {}
#[Swag.Overload]
func toto(x: s8) {}

#test
{
    toto(0)
}