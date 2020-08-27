//#[swag.printbc]
func toto(x: ...)
{
    var xx: [1] s32 = ?
}

#[swag.printbc]
#test
{
    loop 10
        toto(0)
}