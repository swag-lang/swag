//#[Swag.PrintBc, Swag.Optimize("", true)]
func toto(b2: s32)->s32
{
    const A = 128
    const B = 200
    if !(b2 >= A and b2 >= B):
        return 1
    return 2
}

#test
{
    discard toto(1)
}