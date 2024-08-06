//#global testerror "Err0602"

#[Swag.Overload]
func toto(x: s32) {}

#test
{
    toto(0's8)
}