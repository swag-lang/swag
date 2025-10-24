#global testerror "Saf0021"
func toto(x: rune) {}

#[Swag.Safety(.All, .All, true), Swag.Optimize(.All, false)]
#test
{
    toto(-1)
}
