#global testerror "Err0379"
func toto() {}
func titi() throw
{
    throw toto()
}