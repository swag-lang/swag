#global skip
//#global #[Swag.PrintBc]

#[Swag.ConstExpr]
struct String {}

impl String
{
    #[Swag.Implicit]
    mtd opCast()->string
    {
        return "toto"
    }

    #[Swag.Implicit]
    mtd opAffect(value: string)
    {
    }
}

#[Swag.PrintBc]
#[Swag.ConstExpr, Swag.Inline]
func get()->String
{
    return "boundcheck"
}

#[Swag.Safety(get(), false)]
func toto(f: bool) {}
