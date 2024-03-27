#global testerror "San0002"

#[Swag.Safety("", true)]
func toto()->s32
{
    let x = 0
    var y = 1
    y *= x
    var z = 1
    z /= y
    return z
}
