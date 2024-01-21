#global testerror "Err0624"

#[Swag.Macro, Swag.CalleeReturn]
func toto()
{
    return "toto"
}

func titi()
{
    toto()
}