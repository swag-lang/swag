#global testerror "Saf0016"

#[Swag.Sanity(true)]
//#[Swag.PrintGenBc]
func toto()->s32
{
    let x = 0
    var y = 1
    y *= x
    var z = 1
    z /= y
    return z
}