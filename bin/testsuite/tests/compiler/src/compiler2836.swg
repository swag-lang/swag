//#[Swag.PrintBc]
func toto(x: s32)->s32
{
    const A = [1, 2, 3, 4, 5]
    var v = 10 / A[-x]
    return v
}

#test
{
    var r = toto(-2)
}