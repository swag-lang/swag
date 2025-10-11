#global testerror "Saf0021"
func toto(x: rune) {}

#[Swag.Safety("", true), Swag.Optimize("", false)]
#test
{
    toto(-1)
}
