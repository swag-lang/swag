#global testerror "Err0747"

#[Swag.Macro]
func toto()
{
    var #alias0 = 0
}

#test
{
    toto(|x, y|)
}