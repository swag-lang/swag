#global testerror "Err0682"

#[Swag.Macro, Swag.CalleeReturn]
func toto()
{
    return "toto"
}

func titi()
{
    toto()
}