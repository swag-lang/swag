//#[Swag.PrintBc]
func toto(x: s32)->s32
{
    const A = [1, 2, 3, 4, 5]
    let v   = 10 / A[-x]
    return v
}

#test
{
    let r = toto(-2)
}
