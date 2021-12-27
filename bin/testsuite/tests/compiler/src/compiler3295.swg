#[Swag.Mixin]
func TITI(stmt: code)
{
    #mixin stmt
}

TITI() struct X{}

#test
{
    var x: X
}