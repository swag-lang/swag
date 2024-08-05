#global testerror "Err0696"

#[Swag.Macro, Swag.CalleeReturn]
func toto()
{
    return "toto"
}

func titi()
{
    toto()
}