#global testerror "Err0242"
func toto() {}
func titi() throw
{
    throw toto()
}