#global testerror "Err0373"
//#global skip
//#global #[Swag.PrintBc]

using Core

#[Swag.ConstExpr]
func get()->String
{
    return "boundcheck1"
}

#[Swag.Safety(get(), false)]
func toto(f: bool) {}
