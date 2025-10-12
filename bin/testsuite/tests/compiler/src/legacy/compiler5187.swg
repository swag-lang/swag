//#[Swag.PrintBc, Swag.Optimize(.All, true)]
func toto(b2: s32)->s32
{
    const A = 128
    const B = 200
    if !(b2 >= A and b2 >= B) do
        return 1
    return 2
}

#test
{
    discard toto(1)
}
