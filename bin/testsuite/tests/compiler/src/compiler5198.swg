#global testerror "Err0361"
func toto() {}
func titi() throw
{
    throw toto()
}