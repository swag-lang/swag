#global testerror "Err0318"
func toto() {}
func titi() throw
{
    throw toto()
}