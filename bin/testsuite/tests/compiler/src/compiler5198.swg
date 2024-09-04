#global testerror "Err0239"
func toto() {}
func titi() throw
{
    throw toto()
}