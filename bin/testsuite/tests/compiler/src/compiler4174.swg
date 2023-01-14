#[Swag.Safety("", true)]
//#[Swag.PrintBc]
func toto()->s32
{
    x := 1
    y := 2
    y *= x
    z := 4
    z /= y
    return z
}

#test
{
    @assert(toto() == 2)
}