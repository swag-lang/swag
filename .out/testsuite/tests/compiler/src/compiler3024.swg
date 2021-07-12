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
    @print(a.x)
}

#[Swag.printbc, Swag.optim("", false)]
#test
{
    titi(A{round(666)})
}*/