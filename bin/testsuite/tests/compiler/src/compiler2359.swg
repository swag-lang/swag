#global testerror "Err0167"

#[Swag.Overload]
func toto();

var G = 0
#[Swag.Overload]
func toto(x: s32)
{
    G = 1
}

#test
{
    toto()
}