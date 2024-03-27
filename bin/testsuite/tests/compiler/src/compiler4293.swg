#global testerror "Saf0020"
func toto(x: rune)
{
}

#[Swag.Safety("", true), Swag.Optim("", false)]
#test
{
    toto(-1)
}
