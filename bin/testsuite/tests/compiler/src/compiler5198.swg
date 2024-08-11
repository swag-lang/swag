#global testerror "Err0232"
func toto() {}
func titi() throw
{
    throw toto()
}