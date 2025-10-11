struct String
{
    a:   string
}

impl String
{
    #[Swag.Inline]
    mtd opCast()->string
    {
        return me.a
    }
}

#[Swag.Compiler, Swag.ConstExpr]
func GEN()->String
{
    return {}
}

#ast cast(string) GEN()
