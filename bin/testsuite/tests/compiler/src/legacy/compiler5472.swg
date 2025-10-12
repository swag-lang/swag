struct String
{
    val: string
}

impl String
{
    #[Swag.Implicit]
    mtd opAffect(x: string)
    {
        me.val = x
    }
}

//#[Swag.PrintBc, Swag.Optimize(.All, false)]
#[Swag.Inline]
func toto()->String
{
    return "toto"
}

#test
{
    let a = toto()
    @assert(a.val == "toto")
}
