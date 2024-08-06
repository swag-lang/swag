#global testerror "Err0693"

#[Swag.Macro, Swag.CalleeReturn]
func toto()
{
    return "toto"
}

func titi()
{
    toto()
}