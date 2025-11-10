struct A
{
    x:   s32 = 666
}

var gA: A

func getA() => &gA

#test
{
    let val = &getA().x
    @assert(dref val == 666)
}
