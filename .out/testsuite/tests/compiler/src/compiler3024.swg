/*struct A
{
    x: s32
}

#[Swag.inline]
func round(val: s32)->s32
{
    return val * val
}

func titi(a: A)
{
}

#test
{
    titi(A{round(0)})
}*/