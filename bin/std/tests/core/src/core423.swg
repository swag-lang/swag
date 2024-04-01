//#global skip
//#global #[Swag.PrintBc]

using Core

#[Swag.ConstExpr]
func get()->String
{
    return "boundcheck"
}

#[Swag.Safety(get(), false)]
func toto(f: bool) {}
