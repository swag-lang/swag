struct A { x: s32}
impl A
{
    mtd opDrop()
    {
        @print(x)
    }
}

#[Swag.Inline]
func titi()->A
{
    return {}
}

#[Swag.Inline]
func toto(v: A)
{
    @print(v.x)
}

#[Swag.Macro]
func tata(c: code)
{
    #inject c
}

#[Swag.PrintBc]
#test
{
    toto(titi())
    tata()
    {
        var v = 1
        @print(v)
    }
}