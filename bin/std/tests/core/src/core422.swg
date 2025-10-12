using Core

#[Swag.ConstExpr]
func get()->Swag.SafetyWhat
{
    return Swag.SafetyWhat.BoundCheck
}

#[Swag.Safety(.All, get(), false)]
func toto() {}

#test
{
}
