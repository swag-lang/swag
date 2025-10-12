using Core

#[Swag.ConstExpr]
func get()->string
{
    return "boundcheck"
}

#[Swag.Safety(.All, get(), false)]
func toto() {}

#test
{
}
