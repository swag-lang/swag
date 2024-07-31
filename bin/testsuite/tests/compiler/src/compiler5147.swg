#global testerror "Err0703"

#[Swag.Macro, Swag.CalleeReturn]
func toto()
{
    return "toto"
}

func titi()
{
    toto()
}