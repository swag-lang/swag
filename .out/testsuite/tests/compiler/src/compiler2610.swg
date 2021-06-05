attr ife(expr: bool);

#[Swag.constexpr]
func titi()->bool
{
    return true
}

#[ife(titi())]
func toto()
{

}