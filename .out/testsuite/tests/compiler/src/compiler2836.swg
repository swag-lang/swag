//#[Swag.PrintBc]
func toto(x: s32)->s32
{
    const A = @[1, 2, 3, 4, 5]
    v := 10 / A[-x]
    return v
}

#test
{
    r := toto(-2)
}