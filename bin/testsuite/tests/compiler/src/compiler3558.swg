#global skip
#[Swag.Mixin]
func toto()
{
    var @alias0 = 5
}

toto(|b|)

#placeholder b
#test
{
    b = true
}
