#global testerror "Err0244"
func toto() {}
func titi() throw
{
    throw toto()
}