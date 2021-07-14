struct A
{
    x: s32
}

#[Swag.Inline]
func round(val: s32)->s32
{
    return val * val
}

func titi(a: A)
{
    @assert(a.x == 36)
}

#[/*Swag.PrintBc,*/ Swag.Optim("", false)]
#test
{
    titi(A{round(6)})
}