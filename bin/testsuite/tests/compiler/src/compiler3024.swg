struct A
{
    x:   s32
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

#[Swag.Optimize(.All, false)] /*Swag.PrintBc,*/
#test
{
    titi(A{round(6)})
}
