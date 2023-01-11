//#global testerror

//#[Swag.Optim("", false)]
#[Swag.Safety("", false)]
//#[Swag.PrintBc]
func toto()->s32
{
    x := 1
    y := 2
    y += x
    z := 6
    z /= y
    return z
}

#test
{
    @assert(toto() == 2)
}