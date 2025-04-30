struct A { x: s32}
impl A
{
    mtd opDrop()
    {
        @assert(x == 0)
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
func tata(c: code)
{
    #inject c
}

#[Swag.NoInline]
#[Swag.Optimize("", false)]
func tx(x: ...)
{
}

var G = 1
#test
{
    toto(titi())
    tata()
    {
        let v = 1
        tx(v)
    }
}
