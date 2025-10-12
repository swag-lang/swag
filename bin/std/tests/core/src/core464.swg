//#global skip
//#global #[Swag.PrintBc]

using Core

#[Swag.ConstExpr, Swag.Inline]
func get()->String
{
    return "boundcheck"
}

#[Swag.Safety("", cast(string) get(), false)]
func toto(f: bool) {}
