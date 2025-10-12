#global testerror "Saf0021"
func toto(x: rune) {}

#[Swag.Safety(.All, "", true), Swag.Optimize("", false)]
#test
{
    toto(-1)
}
