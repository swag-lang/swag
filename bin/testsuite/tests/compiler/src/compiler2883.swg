#global testerror "Err0738"

#[Swag.Macro]
func toto()
{
    var #alias0 = 0
}

#test
{
    toto(|x, y|)
}