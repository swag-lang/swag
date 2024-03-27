attr ife(expr: bool);

#[Swag.ConstExpr]
func titi()->bool
{
    return true
}

#[ife(titi())]
func toto()
{

}