#global testerror "Err0380"
func toto() {}
func titi() throw
{
    throw toto()
}