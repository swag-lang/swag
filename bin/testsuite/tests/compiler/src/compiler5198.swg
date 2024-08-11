#global testerror "Err0231"
func toto() {}
func titi() throw
{
    throw toto()
}