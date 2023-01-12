#global testerror

#[Swag.Safety("", true)]
func toto()->s32
{
    x := 0
    y := 1
    y *= x
    z := 1
    z /= y
    return z
}