//#global skip
//#global #[Swag.PrintBc]

#[Swag.ConstExpr]
struct String
{
    x:   u64
}

impl String
{
    #[Swag.Implicit, Swag.Inline]
    mtd opCast()->string
    {
        return "toto"
    }

    #[Swag.Implicit]
    mtd opAffect(value: string) {}
}

#[Swag.ConstExpr, Swag.Inline]
func get()->String
{
    return "boundcheck"
}

#[Swag.Safety("", get(), false)]
func toto(f: bool) {}
