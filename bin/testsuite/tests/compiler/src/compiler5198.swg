#global testerror "Err0368"
func toto() {}
func titi() throw
{
    throw toto()
}