attr ife(expr: bool);

#[swag.constexpr]
func titi()->bool
{
    return true
}

#[ife(titi())]
func toto()
{

}