#global testerror "Err0312"
func toto() {}
func titi() throw
{
    throw toto()
}