#global testerror "Err0573"
func toto() {}
func titi() throw
{
    throw toto()
}