#global testerror "Err0272"
func toto() {}
func titi() throw
{
    throw toto()
}