#[Swag.Compiler, Swag.ConstExpr]
func GEN()->string
{
    return "string"
}

#assert(GEN()[0] == 's''u8)
