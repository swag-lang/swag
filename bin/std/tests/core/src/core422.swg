using Core

#[Swag.ConstExpr]
func get()->string
{
    return "boundcheck"
}

#[Swag.Safety(get(), false)]
func toto()
{
}

#test
{
}