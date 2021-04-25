#global testerror

#[swag.macro]
func toto()
{
    var @alias0 = 0
}

#test
{
    toto(|x, y|)
}