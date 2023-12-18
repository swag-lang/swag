#global testerror "Err0053"

#[Swag.Overload]
func toto(x: s32) {}

#test
{
    toto(0's8)
}