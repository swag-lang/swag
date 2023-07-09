#global testerror

#[Swag.Safety("", true)]
func toto()->s32
{
    var x = 0
    var y = 1
    y *= x
    var z = 1
    z /= y
    return z
}