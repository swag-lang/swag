//#global testerror
//#global skip
//#global #[Swag.PrintBc]
/*
using Core, Core.StrConv

#[Swag.ConstExpr]
func get()->String
{
    #ast
    {
        var sb: StringBuilder
        sb.appendString("return \"boundcheck\"")
        return sb.toString()
    }
}

#[Swag.Safety(.All, get(), false)]
func toto(f: bool) {}
*/