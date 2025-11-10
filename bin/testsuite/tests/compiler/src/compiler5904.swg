var g_A: s32

#[Swag.Inline]
func toto()->#null *s32
{
    if g_A do
        return null
    return &g_A
}

func titi(prt: *s32) {}

//#[Swag.PrintBc]
func tata()
{
    let ptr = assume toto()
    titi(ptr)
}

#test
{
    tata()
}
