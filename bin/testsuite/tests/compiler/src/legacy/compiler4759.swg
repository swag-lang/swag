#[Swag.Overload]
func toto(x: s32) {}
#[Swag.Overload]
func toto(y: s8) {}

#test
{
    toto(y: 0)
}
