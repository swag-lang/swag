#global testerror "Err0271"
func toto() {}
func titi() throw
{
    throw toto()
}