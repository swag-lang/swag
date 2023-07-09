#global testerror

#[Swag.ConstExpr]
#[Swag.Safety("", true)]
func toto(v: u8)->u8
{
    return 255'u8 + v
}

#[Swag.ConstExpr]
func titi(v: u8)->u8
{
    return toto(v)
}

#test
{
    var x = titi(1)
}