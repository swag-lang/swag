struct A { x: s32 }
impl A
{
    mtd opDrop()
    {
        @assert(me.x == 0)
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
    @assert(v.x == 0)
}

#[Swag.Macro]
func tata(c: #code void)
{
    #inject(c)
}

#[Swag.NoInline, Swag.Optimize(.All, false)]
func tx(x: ...) {}

var G = 1
#test
{
    {
        toto(titi())
    }

    {
        toto(titi())
    }
}
