#global testerror "Err0343"
func toto() {}
func titi() throw
{
    throw toto()
}
